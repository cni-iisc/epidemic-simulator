# Testing

### Syntax for JSON files for testing protocols

The testing protocol is perhaps the most complicated in the sense that there are many parameters that need to be provided. In order to better understand the parameters, it is best to understand how testing is implemented.

The testing and contact tracing is initiated when an agent goes for a test and it turns out to be positive. To begin with, we assign a probability that a hospitalised agent gets tested and a symptomatic agent gets tested. These are the “initial” index cases. We refer to these index cases as hospitalised_index_case and symptomatic_index_case. There will be a third kind of cases that we shall call positive_index_cases that are cases discovered through contact tracing.

Once an index case is identified, a certain fraction of their close contacts are taken as their “close contacts” and are quarantined. Among the close contacts, some are chosen and tested. Those positive cases that are identified through this process are called positive_index_cases.

Thus, in order to specify the testing protocol, we need to specify the following probabilities:

- Index case probabilities:
  - `prob_test_index_symptomatic`: Probability that a symptomatic agent voluntarily gets a test
  - `prob_test_index_hospitalised`: Probability that a hospitalised agent voluntarily gets a test

- Contact tracing probabilities
  - `prob_contact_trace_project_symptomatic`: The fraction of a symptomatic_index_case’s project members that are chosen as close contacts
  

- Testing probabilities for chosen contacts:
  - `prob_test_household_hospitalised_symptomatic`: The probability that a household-contact of a hospitalised_index_case that happens to symptomatic is given a test
  

### Example JSON
All of this is specified in a json file which is also an array of dictionaries specifying these probabilities. An example is provided here:

```json
[
  {
    "num_days": 100,
    "test_false_negative": 0,
    "prob_test_index_symptomatic" : 0,
    "prob_test_index_hospitalised" : 1.0,
    "prob_retest_recovered" : 0,

    "prob_contact_trace_household_symptomatic" : 0,
    "prob_contact_trace_project_symptomatic" : 0,
    "prob_contact_trace_random_community_symptomatic" : 0,
    "prob_contact_trace_neighbourhood_symptomatic" : 0,
    "prob_contact_trace_class_symptomatic" : 0,

    "prob_contact_trace_household_hospitalised" : 1,
    "prob_contact_trace_project_hospitalised" : 1,
    "prob_contact_trace_random_community_hospitalised" : 1,
    "prob_contact_trace_neighbourhood_hospitalised" : 0.005,
    "prob_contact_trace_class_hospitalised" : 0,

    "prob_contact_trace_household_positive" : 1,
    "prob_contact_trace_project_positive" : 1,
    "prob_contact_trace_random_community_positive" : 1,
    "prob_contact_trace_neighbourhood_positive" : 0.005,
    "prob_contact_trace_class_positive" : 0,

    "prob_test_household_positive_symptomatic" : 1,
    "prob_test_household_hospitalised_symptomatic" : 1,
    "prob_test_household_symptomatic_symptomatic" : 0,
    "prob_test_household_positive_asymptomatic" : 0.45,
    "prob_test_household_hospitalised_asymptomatic" : 0.45,
    "prob_test_household_symptomatic_asymptomatic" : 0,

    "prob_test_workplace_positive_symptomatic" : 0.25,
    "prob_test_workplace_hospitalised_symptomatic" : 0.5,
    "prob_test_workplace_symptomatic_symptomatic" : 0,
    "prob_test_workplace_positive_asymptomatic" : 0.1125,
    "prob_test_workplace_hospitalised_asymptomatic" : 0.225,
    "prob_test_workplace_symptomatic_asymptomatic" : 0,

    "prob_test_random_community_positive_symptomatic" : 0.125,
    "prob_test_random_community_hospitalised_symptomatic" : 0.25,
    "prob_test_random_community_symptomatic_symptomatic" : 0,
    "prob_test_random_community_positive_asymptomatic" : 0.06,
    "prob_test_random_community_hospitalised_asymptomatic" : 0.2,
    "prob_test_random_community_symptomatic_asymptomatic" : 0,

    "prob_test_neighbourhood_positive_symptomatic" : 0.125,
    "prob_test_neighbourhood_hospitalised_symptomatic" : 0.25,
    "prob_test_neighbourhood_symptomatic_symptomatic" : 0,
    "prob_test_neighbourhood_positive_asymptomatic" : 0.06,
    "prob_test_neighbourhood_hospitalised_asymptomatic" : 0.2,
    "prob_test_neighbourhood_symptomatic_asymptomatic" : 0,

    "prob_test_school_positive_symptomatic" : 0,
    "prob_test_school_hospitalised_symptomatic" : 0,
    "prob_test_school_symptomatic_symptomatic" : 0,
    "prob_test_school_positive_asymptomatic" : 0,
    "prob_test_school_hospitalised_asymptomatic" : 0,
    "prob_test_school_symptomatic_asymptomatic" : 0
  }
]
```