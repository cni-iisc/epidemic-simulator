# Setting up the python environment

In order to run the city building script, we would need to get a few python packages. It would be easiest to not mess with the internal python distribution and get these packages on a separate virtual environment.

```bash
$ ~/tutorial/markov_simuls > python3 -m venv edaDev
$ ~/tutorial/markov_simuls > source edaDev/bin/activate
(edaDev) $ ~/tutorial/markov_simuls >
```

### Installing the necessary packages

This will create a separate folder called `edaDev` which is a local sandbox for everything we are doing. All packages that we will now install within the `edaDev` virtual environment will be installed inside this folder and will not affect the whole system. If you are comfortable with Anaconda, there are lot of guides to help you [setup a virtualenv](https://uoa-eresearch.github.io/eresearch-cookbook/recipe/2014/11/20/conda/).

```bash
(edaDev) $ ~/tutorial/markov_simuls > pip install -r staticInst/requirements.txt
```

With these packages installed, we are now ready to actually build the city.

### Known issues
In Windows, we have seen users facing issues with the geopandas library.