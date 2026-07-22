# AHRS

The Attitude and Heading Reference System (AHRS) is the mathematical core of the flight controller. It takes the raw, noisy data from the IMU and fuses it together to calculate the drone's orientation in 3D space.

JanFlight uses the [Mahony Filter](https://ahrs.readthedocs.io/en/latest/filters/mahony.html), it leverages a Proportional-Integral (PI) controller to correct gyroscope drift using the accelerometer's gravity vector.

Here is a breakdown of how the Mahony filter is implemented in the JanFlight.

## 1. The Coordinate System

The orientation is internally represented by a [Quaternion](https://en.wikipedia.org/wiki/Quaternion), and usually as [Euler angles](https://en.wikipedia.org/wiki/Euler_angles) (roll, pitch, yaw) to humans.

JanFlight uses the standard NED (North-East-Down) notation, with its origin as the [Center of Gravity](https://en.wikipedia.org/wiki/Center_of_gravity_of_an_aircraft) of the drone:

- X-axis points forward (North)
- Y-axis points right (East)
- Z-axis points down (Down)

## 2. Mathematical Breakdown

#### 1. Unit Conversion & Normalization is performed.

```C++
gx *= 0.0174533f; gy *= 0.0174533f; gz *= 0.0174533f;
recipNorm = 1.0f / sqrt(ax * ax + ay * ay + az * az);
ax *= recipNorm; ay *= recipNorm; az *= recipNorm;
```

* **Gyroscope**: Converted from degrees/sec to radians/sec (multiplied by $\pi/180$).
* **Accelerometer**: Normalized to a 1G unit vector (pure direction, ignoring magnitude).

#### 2. Estimating the Gravity Vector

```C++
half_vx = q1 * q3 - q0 * q2;
half_vy = q0 * q1 + q2 * q3;
half_vz = q0 * q0 - 0.5f + q3 * q3;
```

Calculates expected gravity in the drone's Body frame by multiplying Earth's gravity $[0, 0, 1]^T$ by the transposed rotation matrix $R(\mathbf{q})^T$:

$$\mathbf{g}_B = \begin{bmatrix} 1 - 2(q_2^2 + q_3^2) & 2(q_1q_2 + q_0q_3) & 2(q_1q_3 - q_0q_2) \\ 2(q_1q_2 - q_0q_3) & 1 - 2(q_1^2 + q_3^2) & 2(q_2q_3 + q_0q_1) \\ 2(q_1q_3 + q_0q_2) & 2(q_2q_3 - q_0q_1) & 1 - 2(q_1^2 + q_2^2) \end{bmatrix} \begin{bmatrix} 0 \\ 0 \\ 1 \end{bmatrix}$$

$$\mathbf{g}_B = \begin{bmatrix} \mathbf{V}_x \\ \mathbf{V}_y \\ \mathbf{V}_z \end{bmatrix} = \begin{bmatrix} 2(q_1q_3 - q_0q_2) \\ 2(q_2q_3 + q_0q_1) \\ 1 - 2(q_1^2 + q_2^2) \end{bmatrix}$$

From the matrix multiplication, the full expected gravity vector for the Z-axis is:

$$\mathbf{V}_z = 1 - 2(q_1^2 + q_2^2)$$

the algorithm calculates the half vector to save processing power, we first divide this by 2, we store this value in variable $half\_vz$:

$$half\_vz = 0.5 - q_1^2 - q_2^2$$

To optimize this further, we use the fundamental property of a unit quaternion. The sum of the squares of its components must always equal 1:

$$q_0^2 + q_1^2 + q_2^2 + q_3^2 = 1$$

We can rearrange this identity to isolate $(q_1^2 + q_2^2)$:

$$q_1^2 + q_2^2 = 1 - q_0^2 - q_3^2$$

Rearranging the terms:

$$half\_vz = q_0^2 - 0.5 + q_3^2$$

!> **Note**: The Mahony filter algebraically simplifies the direct matrix equation $\mathbf{V}_z = 1 - 2(q_1^2 + q_2^2)$ to eliminate slow floating-point multiplications, saving critical processing time on microcontrollers. It compensates for this "half-vector" math by simply doubling the PID gains at startup.

#### 3. Error Calculation

```C++
half_ex = (ay * half_vz - az * half_vy);
half_ey = (az * half_vx - ax * half_vz);
half_ez = (ax * half_vy - ay * half_vx);
```

Finds the rotational error by taking the cross product of measured gravity from sensor (ax, ay, az) and estimated gravity (half_v):

$$\mathbf{E} = \mathbf{a}_{measured} \times \mathbf{g}_{B}$$.
$$\mathbf{g}_B = \begin{bmatrix} half\_ex \\ half\_ey \\ half\_ez \end{bmatrix} = \begin{bmatrix} \hat{i} & \hat{j} & \hat{k} \\ ax & ay & az \\ half\_vx & half\_vy & half\_vz \end{bmatrix}$$

#### 4. The PI Controller

```C++
if(twoKi > 0.0f) {
  integralFBx += twoKi * half_ex * invSampleFreq; // ...
  gx += integralFBx; // ...
}
gx += twoKp * half_ex; // ...
```
The error vector feeds a PI controller to correct the gyro:

* **Proportional (twoKp)**: Applies immediate correction.
* **Integral (twoKi)**: Eliminates long-term drift.

#### 5. Quaternion Integration

```C++
gx *= (0.5f * invSampleFreq); // ...
q0 += (-qb * gx - qc * gy - q3 * gz); // ...
```
Calculates the quaternion derivative $\dot{\mathbf{q}} = \frac{1}{2} \mathbf{q} \otimes \boldsymbol{\omega}$ using the corrected gyro rates. It is integrated over time (dt) to update the 3D orientation, then re-normalized.

#### 6. Euler Angle Conversion

```C++
roll_IMU = atan2(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2) * 57.29577951; // ...
```
Converts the 4D quaternion into human-readable 3D Roll, Pitch, and Yaw angles. The output is converted from radians to degrees (multiplied by $180/\pi$) for the flight control mixer.

#### Question

When we pass the raw sensor data into the Mahony filter in the main loop, you will notice specific positive and negative signs:

```C++
// Uses IMU data to estimate absolute vehicle attitude
Mahony(GyroX, -GyroY, -GyroZ, -AccX, AccY, AccZ, dt);
```

!> Think why these signs may have been added after seeing the mathematical breakdown. **Hint**: Take a look at the reference system of MPU6500.

*Last Updated: 14th July 2026*