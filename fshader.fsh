uniform sampler2D qt_Texture0;
varying highp vec2 qt_TexCoord0;
uniform bool u_highlight;
//varying highp vec4 v_position;
//varying highp vec3 v_normal;
varying highp vec2 v_texture;
uniform vec4 customColor;

void main(void)
{    
    vec4 resColor = customColor.w == 0.0 ? texture2D(qt_Texture0, v_texture) : customColor;

    // Highlithing
    if (u_highlight == true)
    {
      float highlightIntensity = 0.6f;
      vec4 highlightColor = vec4(0.05, 0.78, 0.93, 0.0);
      highlightColor *= highlightIntensity;
      resColor += highlightColor;
    }

    gl_FragColor = resColor;
}
