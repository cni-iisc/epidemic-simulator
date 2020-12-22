# Interventions

## Pre-defined interventions
The following interventions are implemented (along with the integer value that one
needs to provide to run them).

<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;}
.tg td{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;
  overflow:hidden;padding:10px 5px;word-break:normal;}
.tg th{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;
  font-weight:normal;overflow:hidden;padding:10px 5px;word-break:normal;}
.tg .tg-cly1{text-align:left;vertical-align:middle}
.tg .tg-mwxe{text-align:right;vertical-align:middle}
.tg .tg-zt7h{font-weight:bold;text-align:right;vertical-align:middle}
.tg .tg-yla0{font-weight:bold;text-align:left;vertical-align:middle}
</style>
<table class="tg">
<thead>
  <tr>
    <th class="tg-zt7h">Code</th>
    <th class="tg-yla0">Identifier for intervention</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td class="tg-mwxe">0</td>
    <td class="tg-cly1">no_intervention</td>
  </tr>
  <tr>
    <td class="tg-mwxe">1</td>
    <td class="tg-cly1">case_isolation</td>
  </tr>
  <tr>
    <td class="tg-mwxe">2</td>
    <td class="tg-cly1">home_quarantine</td>
  </tr>
  <tr>
    <td class="tg-mwxe">3</td>
    <td class="tg-cly1">lockdown</td>
  </tr>
  <tr>
    <td class="tg-mwxe">4</td>
    <td class="tg-cly1">case_isolation_and_home_quarantine</td>
  </tr>
  <tr>
    <td class="tg-mwxe">5</td>
    <td class="tg-cly1">case_isolation_and_home_quarantine_sd_70_plus</td>
  </tr>
  <tr>
    <td class="tg-mwxe">6</td>
    <td class="tg-cly1">lockdown_fper_ci_hq_sd_70_plus_sper_ci</td>
  </tr>
  <tr>
    <td class="tg-mwxe">7</td>
    <td class="tg-cly1">lockdown_fper</td>
  </tr>
  <tr>
    <td class="tg-mwxe">8</td>
    <td class="tg-cly1">ld_fper_ci_hq_sd65_sc_sper_sc_tper</td>
  </tr>
  <tr>
    <td class="tg-mwxe">9</td>
    <td class="tg-cly1">ld_fper_ci_hq_sd65_sc_sper</td>
  </tr>
  <tr>
    <td class="tg-mwxe">10</td>
    <td class="tg-cly1">ld_fper_ci_hq_sd65_sc_oe_sper</td>
  </tr>
</tbody>
</table>
<br>

## User-defined interventions
The json file for specifying the intervention is expected to be an array of dictionaries. Here is an example that we used in one of our reports:

**Note**: For `CALIBRATION_DELAY` many days, the simulator runs no-intervention and then starts implementing the intervention according to the definition provided in the json file.

```json

[
    {
        "num_days": 24,
        "compliance": 0.6,
        "compliance_hd": 0.4,
        "lockdown": {
            "active": true
        }
    },
    {
        "num_days": 22,
        "compliance": 0.6,
        "compliance_hd": 0.4,
        "lockdown": {
            "active": true
        },
        "mask_factor": 0.8
    },
    {
        "num_days": 17,
        "compliance": 0.6,
        "compliance_hd": 0.4,
        "lockdown": {
            "active": true
        },
        "social_dist_elderly": {
            "active": true
        },
        "mask_factor": 0.8
    },
    {
        "num_days": 14,
        "compliance": 0.6,
        "compliance_hd": 0.4,
        "community_factor": 0.75,
        "case_isolation": {
            "active": true
        },
        "home_quarantine": {
            "active": true
        },
        "social_dist_elderly": {
            "active": true
        },
        "school_closed": {
            "active": true
        },
        "mask_factor": 0.8
    },
    {
        "num_days": 153,
        "compliance": 0.6,
        "compliance_hd": 0.4,
        "community_factor": 0.75,
        "case_isolation": {
            "active": true
        },
        "home_quarantine": {
            "active": true
        },
        "social_dist_elderly": {
            "active": true
        },
        "school_closed": {
            "active": true
        },
        "mask_factor": 0.8,
        "trains": {
            "active": true,
            "fraction_forced_to_take_train": 1
        }
    },
    {
        "num_days": 1,
        "compliance": 0.6,
        "compliance_hd": 0.4,
        "community_factor": 1.0,
        "case_isolation": {
            "active": true
        },
        "home_quarantine": {
            "active": true
        },
        "social_dist_elderly": {
            "active": true
        },
        "school_closed": {
            "active": true
        },
        "mask_factor": 0.8,
        "trains": {
            "active": true,
            "fraction_forced_to_take_train": 1
        },
        "neighbourhood_containment": {
            "active":true,
            "leakage": 1.0,
            "threshold": 0.001
        }
    }
]
```

If the simulator is running for more days than specified in the intervention file, the intervention defined in the last element of the array is used for all subsequent days.

The simulator requires the `num_days` key to specify the number of days that a specific intervention is to be used before moving on to the next block. However, it is more convenient to just specify start and end dates. I use a helper script to build this file from a template the specifies start and end dates.