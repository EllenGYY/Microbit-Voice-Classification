#include "MicroBit.h"
#include "AudioStreamer.h"
#include "StreamNormalizer.h"
#include "Tests.h"


MicroBit &uBit = *(new MicroBit());
static NRF52ADCChannel *mic = NULL;
static AudioStreamer *streamer = NULL;
static StreamNormalizer *processor = NULL;

void stream_mic()
{
    if (mic == NULL){
        mic = uBit.adc.getChannel(uBit.io.microphone);
        mic->setGain(7,0);
    }

    mic->output.setBlocking(true);

    if (processor == NULL)
        processor = new StreamNormalizer(mic->output, 1f, true, DATASTREAM_FORMAT_8BIT_SIGNED);

    if (streamer == NULL)
        streamer = new AudioStreamer(processor->output);

    uBit.io.runmic.setDigitalValue(1);
    uBit.io.runmic.setHighDrive(true);

    while(1){
        uBit.sleep(1);
    }
}

int main()
{
    uBit.sleep(100);
    uBit.init();

    stream_mic();
}