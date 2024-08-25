# INA219

## Notes

1. Bus voltage (v_bus) is the voltage across in- and gnd.
2. Battery voltage, v_bat = v_bus + v_shunt.
3. For above relation to make sense, the load should be connected as shown in the schematic below.
4. Current and power measurement is always zero for uncalibrated sensor. Calibration is performed using <code>ina219_calibrate()</code> function.

```
  _______
         |    v_bat
         |      |
     in+ |=------
         |      |
         |   [shunt]
         |      |
     in- |=------
         |      |
         |    [load]
  INA219 |      |
  -------      GND
```

