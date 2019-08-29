uniform mat4 u_viewMmatrix;
uniform mat4 u_projMmatrix;
uniform mat4 u_modelMmatrix;
attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texture;
varying highp vec4 v_position;
varying highp vec3 v_normal;
varying highp vec2 v_texture;

void main(void)
{
    //convert point via model-view matrix
    //mat4 mvpMatrix = u_modelMmatrix * u_projMmatrix;
  mat4 mvMatrix = u_viewMmatrix * u_modelMmatrix;
  gl_Position = u_projMmatrix * mvMatrix * a_position;

  v_position = mvMatrix * a_position;
  v_normal = normalize(vec3(mvMatrix * vec4(a_normal, 0.0)));
  v_texture = a_texture;
}
