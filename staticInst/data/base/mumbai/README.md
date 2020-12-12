# Details on how this data was obtained

Most of the data was obtained from the [2011 Census tables](https://www.censusindia.gov.in/DigitalLibrary/Archive_home.aspx)

## Geographic boundaries

The geojson files were obtained from [datameet](https://github.com/datameet/Municipal_Spatial_Data/), and updated to include the ward names.

## Demographics, employment and ODMatrix information

The data was collated from the 2011 Census Part A for districts [Mumbai](http://censusindia.gov.in/2011census/dchb/DCHB_A/27/2723_PART_A_DCHB_MUMBAI.pdf) and [Mumbai Suburban](http://censusindia.gov.in/2011census/dchb/DCHB_A/27/2722_PART_A_DCHB_MUMBAI%20SUBURBAN.pdf). This has been detailed in the file [Mumbai_master.xlsx](Mumbai_master.xlsx) with a reference for each column and where it was obtained from.

The origin-destination matrix was obtained from a [World Bank Report (2005)](http://documents.worldbank.org/curated/en/891271468258270484/pdf/wps3693.pdf) which provided Zone-wise travel distributions. This has been extrapolated to ward-wise information by assuming that destination workplaces are distributed within a zone according to the ward areas.

The csv files in this directory are generated using the ipynb script provided in [GenerateCSVs.ipynb](GenerateCSVs.ipynb).

Details about slums in Mumbai were obtained from the [BMC Census](https://portal.mcgm.gov.in/irj/go/km/docs/documents/MCGM%20Department%20List/Public%20Health%20Department/Docs/Census%20FAQ%20%26%20Answer.pdf).

## cityProfile.json

### Age and household distributions

The age distribution was computed by extracting the relevant data from the [2011 Census table C13](https://censusindia.gov.in/2011census/C-series/C-13.html) for Maharashtra.

The age distribution was computed by extracting the relevant data from the [2011 Census table HH04](https://censusindia.gov.in/2011census/Hlo-series/HH04.html) for Maharashtra.

The ipynb script for this extraction is provided in [here](raw/maharashtra-census-data/get_age_and_household_size_distributions.ipynb).

### School-size distribution

We do not have data on distribution on school sizes in Mumbai. We use the distribution for Thailand from the paper of [Ferguson et al](https://www.nature.com/articles/nature04017).
