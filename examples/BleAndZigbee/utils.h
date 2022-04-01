enum class SketchState {
  Bluetooth = 0,
  Zigbee = 1
};

unsigned int getNumLights();

bool setNumLights(unsigned int num_lights);

bool isConfigured();

int setupZigbee();

int setupBluetooth();

SketchState sketchState();

void setSketchState(SketchState state);

int setupBluetooth();