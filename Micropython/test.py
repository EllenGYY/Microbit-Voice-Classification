"""Test code."""


from microbit import display, Image, sleep, microphone, SoundEvent


while True:
    display.scroll('UNC Comp Sci Rocks!')
    boat = Image("05500:05550:00500:99999:09990")
    display.show(boat)
    sleep(2000)
    # if microphone.current_event() == SoundEvent.LOUD:
    #     display.show(Image.HEART)
    # elif microphone.current_event() == SoundEvent.QUIET:
    #     display.show(Image.HEART_SMALL)
    # sleep(200)