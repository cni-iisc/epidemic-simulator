# Workplace attendance

When the city is built, the workplaces are split into 6 different types (though this is not used at all!). The attendance json file is supposed to provide the attendance probability for each of days of the simulation (including the initial days corresponding to ``CALIBRATION_DELAY``). The json file is expected to be be an array of dictionaries specifying these probabilities for each day. An example is as follows:

```json
[
    {
        "id": 0,
        "0": 1.0,
        "1": 1.0,
        "2": 1.0,
        "3": 1.0,
        "4": 1.0,
        "5": 1.0,
    },
    {
        "id": 1,
        "0": 1.0,
        "1": 1.0,
        "2": 1.0,
        "3": 1.0,
        "4": 1.0,
        "5": 1.0,
    }
]
```

### Some important nuances

As mentioned earlier, the first entry of the array corresponds to the first day of the simulation (which will often be within the ``CALIBRATION_DELAY`` period).
During the lockdown, all workplaces interactions are dampened except for a small leakage for essential services. Therefore, reduced attendance is already taken into account. If you additional have smaller attendance probability during the lockdown period, it further dampens these values and that’s probably not what you want to do.

Once again, I usually write a script to generate the required attendance file.
