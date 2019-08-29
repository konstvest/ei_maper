uniform sampler2D qt_Texture0;
varying highp vec2 qt_TexCoord0;
uniform bool u_highlight;
uniform bool u_bUseColor;
//varying highp vec4 v_position;
//varying highp vec3 v_normal;
varying highp vec2 v_texture;

void main(void)
{    
    vec4 resColor = u_bUseColor ? vec4(1.0, 0.0, 0.0, 1.0) : texture2D(qt_Texture0, v_texture);

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
