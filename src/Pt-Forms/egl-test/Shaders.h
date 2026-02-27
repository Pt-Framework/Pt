#ifndef Pt_Forms_Shaders_H
#define Pt_Forms_Shaders_H

namespace Pt {

namespace Forms {

// Texture shader
static const char texture_vsh[] = 
"attribute vec3 position; \n"  
"attribute vec3 texCoord; \n"
"uniform mat4 projection; \n"
"uniform mat4 model;      \n"
"varying vec2 v_texCoord; \n"    
"void main() { \n"                   
"  gl_Position = projection * model * vec4( position, 1.0 ); \n"
"  v_texCoord = texCoord.xy; \n"  
"}";                   
  
static const char texture_fsh[] = 
"precision mediump float; \n" 
"varying vec2 v_texCoord; \n"                          
"uniform sampler2D s_tex; \n"                     
"void main() { \n"         
"  gl_FragColor = texture2D( s_tex, v_texCoord ); \n"
"}";

//// Color shader
//static const char color_vsh[] = 
//"attribute vec3 position; \n"  
//"attribute vec4 color; \n"
//"varying vec4 fragmentColor; \n"
//"uniform mat4 projection; \n"
//"uniform mat4 model;      \n"
//"void main() { \n"
//"  fragmentColor = color; \n"
//"  gl_Position = projection * model * vec4( position, 1.0 ); \n"
//"}";                   
//  
//static const char color_fsh[] = 
//"precision mediump float; \n" 
//"varying vec4 fragmentColor; \n"                                           
//"void main() { \n"         
//"  gl_FragColor = fragmentColor; \n"
//"}";


// Color shader
static const char color_vsh[] = 
"attribute vec3 position; \n"  
"uniform mat4 projection; \n"
"uniform mat4 model;      \n"
"void main() { \n"
"  gl_Position = projection * model * vec4( position, 1.0 ); \n"
"}";                   
  
static const char color_fsh[] = 
"precision mediump float; \n" 
"uniform vec4 color; \n"                                        
"void main() { \n"         
"  gl_FragColor = color; \n"
"}";


// line shader
static const char line_vsh[] = 
"attribute vec3 position; \n"  
"attribute vec4 color; \n"
"uniform mat4 projection; \n"
"uniform mat4 model;      \n"
"void main() { \n"                   
"  gl_Position = projection * model * vec4( position, 1.0 ); \n"
"  gl_TexCoord[0].xyz = gl_MultiTexCoord0.xyz; \n"
"  gl_FrontColor = gl_Color; \n"
"}";


// round cap line fragment shader
static const char line_round_fsh[] =                                         
"void main() { \n"         
"  vec4 color = gl_Color; \n"
"  vec3 p = gl_TexCoord[0].xyz; \n"
"  float dist; \n"
"  if( p.z < 0.0 ) { \n"
"     float xdist = min(abs(p.x),abs(p.x-1.0)); \n"
"     float xwidth = fwidth(xdist); \n"
"     float x = smoothstep(1.0-xwidth, 1.0+xwidth, xdist); \n"
"     float ydist = abs(p.y); \n"
"     float ywidth = fwidth(ydist); \n"
"     float y = smoothstep(1.0-ywidth, 1.0+ywidth, ydist); \n"
"     float alpha = 1.0-max(x,y); \n"
"     gl_FragColor = vec4(color.rgb, color.a*alpha); \n"
"     return;  \n"
"  } \n"
"  if( (p.x < 0.0) ) \n"
"     dist = sqrt(p.x*p.x+p.y*p.y); \n"
"  else if( (p.x > 1.0) ) \n"
"     dist = sqrt((p.x-1.0)*(p.x-1.0)+p.y*p.y); \n"
"  else \n"
"     dist = abs(p.y); \n"
"  float width = fwidth(dist); \n"
"  if( abs(p.z) <= 1.0) // alpha coverage \n"
"     width = dist; \n"
"  float alpha = smoothstep(1.0+width,1.0-width, dist); \n"
"  gl_FragColor = vec4(color.rgb, color.a*alpha); \n"
"}";



/*
// line shader
static const char line_vsh[] = 
"attribute vec3 position; \n"  
"attribute vec4 color; \n"
"varying vec4 fragmentColor; \n"
"uniform mat4 projection; \n"
"uniform mat4 model;      \n"
"void main() { \n"                   
"  gl_Position = projection * model * vec4( position, 1.0 ); \n"
"  gl_TexCoord[0].xyz = gl_MultiTexCoord0.xyz; \n"
"  fragmentColor = color; \n"
"}";


// round cap line fragment shader
static const char line_round_fsh[] =                                         
"void main() { \n"         
"  varying vec4 fragmentColor; \n" 
"  vec3 p = gl_TexCoord[0].xyz; \n"
"  float dist; \n"
"  if( p.z < 0.0 ) { \n"
"     float xdist = min(abs(p.x),abs(p.x-1.0)); \n"
"     float xwidth = fwidth(xdist); \n"
"     float x = smoothstep(1.0-xwidth, 1.0+xwidth, xdist); \n"
"     float ydist = abs(p.y); \n"
"     float ywidth = fwidth(ydist); \n"
"     float y = smoothstep(1.0-ywidth, 1.0+ywidth, ydist); \n"
"     float alpha = 1.0-max(x,y); \n"
"     gl_FragColor = vec4(fragmentColor.rgb, fragmentColor.a*alpha); \n"
"     return;  \n"
"  } \n"
"  if( (p.x < 0.0) ) \n"
"     dist = sqrt(p.x*p.x+p.y*p.y); \n"
"  else if( (p.x > 1.0) ) \n"
"     dist = sqrt((p.x-1.0)*(p.x-1.0)+p.y*p.y); \n"
"  else \n"
"     dist = abs(p.y); \n"
"  float width = fwidth(dist); \n"
"  if( abs(p.z) <= 1.0) // alpha coverage \n"
"     width = dist; \n"
"  float alpha = smoothstep(1.0+width,1.0-width, dist); \n"
"  gl_FragColor = vec4(fragmentColor.rgb, fragmentColor.a*alpha); \n"
"}";

*/

} // namespace Forms

} // namespace Pt


#endif // include guard