enum class SketchState {
  Bluetooth = 0,
  Zigbee = 1
};

const char* dimmableLightColorName(unsigned int led);

void dimmableLightPrintInfo(int led, unsigned char brightness_level);

unsigned int getNumLights();

bool setNumLights(unsigned int num_lights);

bool isConfigured();

int setupBluetooth();

SketchState sketchState();

void setSketchState(SketchState state);

int setupBluetooth();

int setupZigbee();
