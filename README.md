# GROWL

GROWL is an approximate implementation of the 
formant filter used in the iOS vocal synthesizer, HOWL. 

The GROWL algorithm itself depends on 
[Soundpipe](http://www.github.com/paulbatchelor/soundpipe.git).
[Sporth](http://www.github.com/paulbatchelor/sporth.git) is 
required to use the Sporth plugin.

By default, GROWL compiles to a Sporth plugin. 
This can be compiled by simply running "make". 

To hear what the filter sounds like, run the Sporth patch 
included with "sporth test.sp". This will generated an audio
file called test.wav that you can then listen to.
