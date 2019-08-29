uniform sampler2D qt_Texture0;
varying highp vec2 qt_TexCoord0;
uniform highp vec4 u_lightPosition;
uniform highp float u_lightPower;
uniform highp vec4 u_color;
uniform highp vec3 u_watcherPos;
uniform highp vec4 u_lightColor;
uniform bool u_highlight;

varying highp vec4 v_position;
varying highp vec3 v_normal;
varying highp vec2 v_texture;

void main(void)
{
  // Ambient
  float ambientStrength = 0.1f;
  vec3 ambient = ambientStrength * u_lightColor;

  // Diffuse
  vec3 norm = normalize(v_normal);
  vec3 lightDir = normalize(u_lightPosition - v_position);
  float diff = max(dot(v_normal, lightDir), 0.0f);
  vec3 diffuse = diff * u_lightColor;

  //vec4 resColor = vec4((ambient + diffuse) * u_color, 1.0f);
  vec4 texColor = texture2D(qt_Texture0, v_texture);
  vec4 resColor = vec4((ambient + diffuse), 1.0f) * texColor;



  gl_FragColor = resColor;

}
