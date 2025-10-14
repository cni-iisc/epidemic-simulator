import pandas as pd
import sys
import argparse
import plotly
import plotly.express as px
import geopandas as gpd

def readWardInfected(input_dir):
    wardinfected_df = pd.read_csv(input_dir + "ward_infected.csv")
    temp = {}
    temp["wardIndex"] = range(len(wardinfected_df))
    temp["num_infected"] = wardinfected_df["ward_infected"]
    wardinfected_output_df = pd.DataFrame(temp)
    wardinfected_output_df.to_csv(input_dir + "ward_infected_map.csv", index = False)
    return wardinfected_output_df

def readGeoJSON(geo_file):
    map_df = gpd.read_file(geo_file)
    #map_df.to_crs(pyproj.CRS.from_epsg(4326), inplace=True)
    return map_df

def chloropethMap(input_dir, geo_file, output_dir):
    wardinfected_df = readWardInfected(input_dir)
    map_df = readGeoJSON(geo_file)
    data = map_df.merge(wardinfected_df, left_on=['wardIndex'], right_on=['wardIndex'])
    fig = px.choropleth(data, geojson = data.geometry, locations = data.index, color = "num_infected", hover_name = data["wardName"], color_continuous_scale = "Viridis")
    fig.update_geos(fitbounds = "locations", visible = True)
    fig.update_layout(title_text = "Cumulative Infected by Ward")
    fig.update(layout = dict(title=dict(x=0.5)))
    fig.update_layout(margin={"r":0,"t":30,"l":10,"b":10}, coloraxis_colorbar={'title':'num_infected'})
    fig.show()
    plotly.offline.plot(fig, filename = output_dir + 'ward_infected_map.html')


def main():
    default_input_path = "../staticInst/data/bangalore-10k-output/"
    default_output_path = "../staticInst/data/bangalore-10k-output/"
    default_geo_file = "../staticInst/data/base/bangalore/city.geojson"
    my_parser = argparse.ArgumentParser(description='Create a chloropeth map for infections in each ward')
    my_parser.add_argument('-i', help='input folder for ward infection data', default=default_input_path)
    my_parser.add_argument('-o', help='output folder', default=default_output_path)
    my_parser.add_argument('-g', help = 'input folder for geojson file', default = default_geo_file)
    args = my_parser.parse_args()
    input_dir = args.i
    output_dir = args.o
    geo_file = args.g
    if len(sys.argv)==1:
        print("No arguments passed.\n")
        my_parser.print_help()
        print("\n Assuming default values.\n")
    chloropethMap(input_dir, geo_file, output_dir)



if __name__ == "__main__":
    main()
