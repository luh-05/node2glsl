// --- GENERATED CODE, DO NOT EDIT ---
// Global pp definitions

#define VALUE float
#define VECTOR vec3
#define RGBA vec4
#define ROTATION vec3

// Global var definitions

VALUE val_0x55555561c2d0;
VALUE val_0x55555561c690;
VALUE val_0x55555561bb90;
VALUE val_0x55555561bd60;
VECTOR val_0x555555612bf0;

// Entry Point
void p3d_main() {
  // Modules

  // Module '0x55555561bf50'
  {
    val_0x55555561bd60 = 9.699999809265137;
  }

  // Module '0x55555561c1e0'
  {
    val_0x55555561c2d0 = 0.5;
  }

  // Module '0x55555561c5e0'
  {
    val_0x55555561c690 = 1.6999999284744263;
  }

  // Module '0x555555612ab0'
  {
    val_0x555555612bf0 = p3d_position;
  }

  // Module '0x555555613760'
  {
    val_0x55555561bb90 = length(val_0x555555612bf0) + val_0x55555561bd60;
  }

  // Module '0x555555612f30'
  {
    p3d_sdf = val_0x55555561bb90;
    mollusk_debug0 = vec3(val_0x55555561c690, val_0x55555561c690, val_0x55555561c690);
    mollusk_debug1 = vec4(val_0x55555561c690, val_0x55555561c690, val_0x55555561c690, 1.0f);
    mollusk_debug2 = vec3(val_0x55555561c690, val_0x55555561c690, val_0x55555561c690);
  }
}
