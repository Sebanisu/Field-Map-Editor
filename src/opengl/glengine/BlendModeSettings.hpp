#ifndef B8E37988_0948_4FCB_91EE_39C46824C86A
#define B8E37988_0948_4FCB_91EE_39C46824C86A
namespace glengine
{
struct BlendModeSettings
{
     static void enable_blending();
     static void default_blend();
     static void add_blend();
     static void subtract_blend();
     static void multiply_blend();
     BlendModeSettings() = delete;
};
}// namespace glengine

#endif /* B8E37988_0948_4FCB_91EE_39C46824C86A */
