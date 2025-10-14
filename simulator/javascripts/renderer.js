define(['model/constants'], function (Constants) {
    "use strict";
    return {



        //To what extent does a family comply with an intervention? 1 = full compliance, 0 = no compliance.
        set_compliance: function () {
            var val = 1;
            switch (INTERVENTION) {
                case CALIBRATION:
                    val = 0.9; //No effect.
                    break;
                case NO_INTERVENTION:
                    val = 0.9; //No effect.
                    break;
                case CASE_ISOLATION:
                    val = 0.7;
                    break;
                case HOME_QUARANTINE:
                    val = 0.5;
                    break;
                case LOCKDOWN:
                    val = 0.9;
                    break;
                case CASE_ISOLATION_AND_HOME_QUARANTINE:
                    val = 0.7;
                    break;
                case CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS:
                    val = 0.7;
                    break;
                case LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI:
                    val = 0.9;
                    break;
                case LOCKDOWN_21:
                    val = 0.9;
                    break;
                case LD_21_CI_HQ_SD70_SC_21_SC_42:
                    val = 0.9;
                    break;
                case LD_21_CI_HQ_SD70_SC_21:
                    val = 0.9;
                    break;
                case LD_21_CI_HQ_SD70_SC_OE_30:
                    val = 0.9;
                    break;
                default:
                    val = 1;
            }
            return val;
        },

        set_default_values_html: function (Constants) {
            document.getElementById("numDays").value = NUM_DAYS;
            document.getElementById("initFrac").value = INIT_FRAC_INFECTED;
            document.getElementById("Incubation").value = 2 * INCUBATION_PERIOD;
            document.getElementById("asymptomaticMean").value = MEAN_ASYMPTOMATIC_PERIOD;
            document.getElementById("symptomaticMean").value = MEAN_SYMPTOMATIC_PERIOD;
            document.getElementById("symtomaticFraction").value = SYMPTOMATIC_FRACTION;
            document.getElementById("meanHospitalPeriod").value = MEAN_HOSPITAL_REGULAR_PERIOD;
            document.getElementById("meanICUPeriod").value = MEAN_HOSPITAL_CRITICAL_PERIOD;
            document.getElementById("compliance").value = this.set_compliance();
            document.getElementById("betaHouse").value = BETA_H;
            document.getElementById("betaWork").value = BETA_W;
            document.getElementById("betaCommunity").value = BETA_C;
            document.getElementById("betaSchools").value = BETA_S;
            document.getElementById("betaPT").value = BETA_PT;

        },




    }


});