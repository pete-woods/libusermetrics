Welcome to Libusermetrics {#mainpage}
=========================

Libusermetrics enables apps to locally store interesting numerical data
for later presentation.  For example in the Ubuntu Greeter "flower"
infographic. 

 - All data is stored locally in /var/usermetrics/.
 - No data is centrally collected via a web-serivice or otherwise, and
   no data is sent over the internet.

The only data that can be stored is numerical, for example "number of
e-mails" or "number of pictures taken". No personally identifying
information is stored using this library.

**WARNING:** This API should be considered private, and is only for use by
first party applications at present. It may change or be removed at any
time.

Getting and installing libusermetrics
-------------------------------------

### Using pre-built packages

If you just want to try out libusermetrics, or write user metrics sources,
then the easiest way is to use the pre-built packages:

 - \ref installing_prebuilt

### Building and installing from source

If you are curious about libusermetrics's internals or intend to contribute to
it, you should get the source and build it:

 - \ref building_source

Getting involved
----------------

The best place to ask questions and discuss about libusermetrics is the \#ubuntu-touch
IRC channel on freenode. 

The libusermetrics project is hosted on Launchpad: https://launchpad.net/libusermetrics

Please file bug reports at: http://bugs.launchpad.net/libusermetrics

Writing metric data sources
---------------------------

For the usermetrics service to know your application is providing data, a click hook is required.
At present, no data is read from the hook, so an empty file registered as follows is sufficient:

    "hooks": {
        "camera": {
            ...
            "usermetrics": "usermetrics.json"
        }
    }

You then have the choice of either using libusermetricsinput to write your data files, or
creating completely custom files with your own code.

### Using libusermetricsinput

 - \ref UserMetricsInput "Libusermetrics Input API Documentation"
 
For simple metrics which only want to increment a counter, see the following examples:
 
 - \subpage MetricManagerIncrementToday.cpp "MetricManagerIncrementToday.cpp: A simple Qt-based metric"
 - \subpage MetricManagerIncrementTodayCAPI.c "MetricManagerIncrementTodayCAPI.c: A simple C-based metric"
 
For simple metrics which only want to update today's value, but cannot simply increment it,
see the following examples:

 - \subpage MetricManagerUpdateToday.cpp "MetricManagerUpdateToday.cpp: A simple Qt-based metric"
 - \subpage MetricManagerUpdateTodayCAPI.c "MetricManagerUpdateTodayCAPI.c: A simple C-based metric"

For more sophisticated metrics, which wish to update the history of the metric, for
example if the user has been offline for a week. Please see the following examples:

 - \subpage MetricManagerAdvanced.cpp "MetricManagerAdvanced.cpp: A Qt-based metric"
 - \subpage MetricManagerAdvancedCAPI.c "MetricManagerAdvancedCAPI.c: A C-based metric"

### Manually
To create completely custom data sources, simply have your application write files into the
following directory:

    ~/.cache/${APP_ID}/usermetrics/filename.foo

It is important that these files are written atomically, or your data could be corrupted. For
example, libusermetricsinput library writes a new file to:

    ~/.cache/${APP_ID}/usermetrics/.tmp/tempfile

and then moves it into the parent directory, ensuring atomic changes.

Writing an infographic visualizer
---------------------------------

An infographic visualizer is simply a binary that takes a one or more command line parameters
that are the paths to infographic data sources, and produces SVG data on its standard output.

Visualizers can specify which data sources they are interested using "input" section of their
click manifest. The first thing to specify is the application ID. This can either be an
application's short ID, or the wildcard '*', referring to all applications. The second part is
a list of regular expressions for which particular files the visualizer is interested in.

    "input": {
        "${SHORT_APP_ID}": [
            "${FILE_REGULAR_EXPRESSION}",
            "${FILE_REGULAR_EXPRESSION_2}"
        ],
        "*": [
            "${FILE_REGULAR_EXPRESSION}",
            "${FILE_REGULAR_EXPRESSION_2}"
        ]
    }

### Iterative visualizer

Iterative visualizers will be called once for each input file. In the example below the echo
command would be called for every application providing a data source, and for each file it
produces that ends in the extension 'libusermetrics.json'. Libusermetricsinput produces file
names with this format, so that visualizers can easily pick them out.

    {
        "exec": "/bin/echo",
        "type": "iterate",
        "input": {
            "*": [
                ".*.libusermetrics.json"
            ]
        }
    }

### Aggregating visualizer

Aggregating visualizers are called once for all inputs at once. They will not be called until
every specified input exists. From that point on they will be called if any individual data
source is updated.

    {
        "exec": "/bin/cat",
        "type": "aggregate",
        "input": {
            "com.ubuntu.camera": [
                "camera-photos.libusermetrics.json",
                "camera-videos.libusermetrics.json"
            ],
            "foo": [
                "source-id.libusermetrics.json"
            ]
        }
    }

Writing a presentation application
----------------------------------

To write a metric presentation application, there is a Qt API.

 - \ref UserMetricsOutput "Libusermetrics Output API Documentation"
 - A good example to refer to is the
   [infographics](https://bazaar.launchpad.net/~unity-team/unity8/trunk/view/head:/Greeter/Infographics.qml)
   for the Unity8 greeter.
