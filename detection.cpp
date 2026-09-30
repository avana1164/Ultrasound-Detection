#include <Wire.h>

const uint8_t MPU_ADDR = 0x68;

// -----------------------------
// DEMO THRESHOLDS
// Change these experimentally
// -----------------------------

// Maximum comfortable rotational speed
const float MAX_GYRO = 30.0;   // degrees/second

// Maximum acceleration change
const float MAX_ACCEL_CHANGE = 0.30;  // g

// Desired probe orientation
const float TARGET_ROLL = 0.0;
const float TARGET_PITCH = 0.0;

// Allowed orientation error
const float ANGLE_TOLERANCE = 15.0;


// Previous acceleration
float previousAx = 0;
float previousAy = 0;
float previousAz = 1;


// ----------------------------------
// Read MPU-6050
// ----------------------------------

bool readMPU(
  int16_t &ax,
  int16_t &ay,
  int16_t &az,
  int16_t &gx,
  int16_t &gy,
  int16_t &gz
) {

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(MPU_ADDR, (uint8_t)14, true) != 14) {
    return false;
  }

  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();

  // Temperature
  Wire.read();
  Wire.read();

  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();

  return true;
}


// ----------------------------------
// Setup
// ----------------------------------

void setup() {

  Serial.begin(115200);
  Wire.begin();

  delay(1000);

  // Wake MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission();

  // Gyroscope ±250 deg/s
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(0);
  Wire.endTransmission();

  // Accelerometer ±2g
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);
  Wire.write(0);
  Wire.endTransmission();

  Serial.println("================================");
  Serial.println(" Ultrasound Probe Monitor");
  Serial.println("================================");
  Serial.println();

  delay(1000);
}


// ----------------------------------
// Main loop
// ----------------------------------

void loop() {

  int16_t rawAx, rawAy, rawAz;
  int16_t rawGx, rawGy, rawGz;

  if (!readMPU(
        rawAx,
        rawAy,
        rawAz,
        rawGx,
        rawGy,
        rawGz
      )) {

    Serial.println("ERROR: MPU-6050 not responding");
    delay(500);
    return;
  }


  // ----------------------------------
  // Convert accelerometer
  // ----------------------------------

  float ax = rawAx / 16384.0;
  float ay = rawAy / 16384.0;
  float az = rawAz / 16384.0;


  // ----------------------------------
  // Convert gyroscope
  // ----------------------------------

  float gx = rawGx / 131.0;
  float gy = rawGy / 131.0;
  float gz = rawGz / 131.0;


  // ----------------------------------
  // Calculate orientation
  // ----------------------------------

  float roll =
    atan2(ay, az) * 180.0 / PI;

  float pitch =
    atan2(
      -ax,
      sqrt(ay * ay + az * az)
    ) * 180.0 / PI;


  // ----------------------------------
  // Calculate acceleration change
  // ----------------------------------

  float accelerationChange =
    sqrt(
      pow(ax - previousAx, 2) +
      pow(ay - previousAy, 2) +
      pow(az - previousAz, 2)
    );

  previousAx = ax;
  previousAy = ay;
  previousAz = az;


  // ----------------------------------
  // Calculate rotation speed
  // ----------------------------------

  float rotationSpeed =
    sqrt(
      gx * gx +
      gy * gy +
      gz * gz
    );


  // ==================================
  // OUTPUT
  // ==================================

  Serial.println("--------------------------------");

  Serial.print("Acceleration: ");
  Serial.print(ax, 2);
  Serial.print("  ");
  Serial.print(ay, 2);
  Serial.print("  ");
  Serial.println(az, 2);

  Serial.print("Rotation:     ");
  Serial.print(gx, 1);
  Serial.print("  ");
  Serial.print(gy, 1);
  Serial.print("  ");
  Serial.println(gz, 1);

  Serial.print("Angle:        Roll ");
  Serial.print(roll, 1);
  Serial.print("°  Pitch ");
  Serial.print(pitch, 1);
  Serial.println("°");


  // ==================================
  // ACCELEROMETER CHECK
  // ==================================

  if (accelerationChange > MAX_ACCEL_CHANGE) {

    Serial.println("⚠ WARNING: Large movement detected");

  } else {

    Serial.println("✓ Movement: Stable");
  }


  // ==================================
  // GYROSCOPE CHECK
  // ==================================

  if (rotationSpeed > MAX_GYRO) {

    Serial.println("⚠ WARNING: Probe rotated too quickly");

  } else {

    Serial.println("✓ Rotation: Controlled");
  }


  // ==================================
  // ANGLE CHECK
  // ==================================

  float rollError =
    abs(roll - TARGET_ROLL);

  float pitchError =
    abs(pitch - TARGET_PITCH);


  if (rollError > ANGLE_TOLERANCE) {

    Serial.println("⚠ WARNING: Roll angle outside target");

  } else {

    Serial.println("✓ Roll angle: OK");
  }


  if (pitchError > ANGLE_TOLERANCE) {

    Serial.println("⚠ WARNING: Pitch angle outside target");

  } else {

    Serial.println("✓ Pitch angle: OK");
  }


  // ==================================
  // OVERALL STATUS
  // ==================================

  bool movementOK =
    accelerationChange <= MAX_ACCEL_CHANGE;

  bool rotationOK =
    rotationSpeed <= MAX_GYRO;

  bool angleOK =
    rollError <= ANGLE_TOLERANCE &&
    pitchError <= ANGLE_TOLERANCE;


  if (movementOK && rotationOK && angleOK) {

    Serial.println();
    Serial.println("        ✓ PROBE POSITION OK");
    Serial.println();

  } else {

    Serial.println();
    Serial.println("        ⚠ ADJUST PROBE");
    Serial.println();
  }


  delay(100);
}
