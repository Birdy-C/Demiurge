 #version 150 
 uniform mat4 matWVP; 
 in      vec4 Position; 
 in      vec4 Color; 
 in      vec2 TexCoord; 
 out     vec2 oTexCoord; 
 out     vec4 oColor; 
 void main() 
 { 
    gl_Position = (matWVP * Position); 
    oTexCoord   = TexCoord; 
    oColor.rgb  = pow(Color.rgb, vec3(2.2));    // convert from sRGB to linear
    oColor.a    = Color.a; 
 } ;