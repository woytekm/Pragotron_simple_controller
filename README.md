# Pragotron_simple_controller
This is a code for nRF52840 Supermini to control slave Pragotron clock (60V)

This is a very simple project. It only creates clock hand moving impulse every second. No NTP sync, no fancy stuff. I like it that way.
As this is for 60V clock, it needs additonal step up converter generating around 40V (this is enough to move hands of 60V PS-1 movement). 
So we have to enable converter, and then enable motor controller with polarity control, alternating polarity with each impulse. 

This is an Adruino Studio project, with nRF52840 Supermini support enabled in Arduino Studio. 
Enabling nRF52840 support in Arduino Studio on OS X was a huge pain in the ass. I don't even remember all steps i had to take to make it work. It took at lease one nightmarish evening. 
This code should be converted to native nRF52840 C dev kit.

