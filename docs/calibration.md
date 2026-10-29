# Calibration notes

Do this before letting the robot walk. A wrong center on one servo is enough
to make it fall over on the first step.

## 1. Center the horns

1. Power the PCA9685 V+ from a separate 5-6 V supply (2 A or more). The Nano
   USB port cannot run eight servos.
2. Flash the firmware with the servo horns removed. At boot every joint goes
   to the home pose one by one.
3. Send `stop`, wait for `ok`, then send `j <n> 0` for each servo n = 0..7.
   Every joint is now at its 0 deg pulse (the `center_us` in `servo.c`).
4. Put the horns on as close to the zero position as the spline allows.
   The spline has 20 or 25 teeth, so you will be off by up to ~7 deg.

## 2. Trim

1. For each joint, use `trim <n> <us>` until the bracket is straight:
   - hip and knee: thigh and shin in a vertical line
   - ankle: sole parallel to the table
   - roll: sole flat side to side
2. About 11 us is one degree on an MG90S. Go in steps of 5-10 us.
3. Send `cal` and copy the center values into `g_servo_cal` in
   `lib/biped_core/servo.c`. Trims are not saved to EEPROM (see README).

## 3. Direction

Right side servos are mounted mirrored, so their `dir` is -1. Check with
`j 0 20` and `j 4 20`: both thighs should swing forward. If one goes back,
flip its `dir`.

## 4. PCA9685 oscillator

The PCA9685 internal oscillator is specified as 25 MHz but real chips are
often a few percent off. If it is off, every pulse is off by the same
percentage and the servos will not match the angles in the gait table.

To check: set one channel to 1500 us and measure the pulse width with a
scope or a logic analyzer. Then

    real_osc = 25 MHz * (measured frame rate / 50 Hz)

or just adjust `PCA_OSC_HZ` in `src/main.cpp` until the measured pulse is
1500 us. The value in the repo (26.1 MHz) is only a starting point.

## 5. Limits

`min_deg` / `max_deg` in the table are there to stop a bad command from
driving a joint into the frame. If you change the leg brackets, re-check
them by moving each joint slowly with `j` and noting where it touches.

The host tests check that every key pose in the walk table stays inside
these limits, so run `make test` after changing either one.
