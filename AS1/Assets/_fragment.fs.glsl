#version 410 core                                                  
                                                                   
out vec4 color;                                                    
                                                                   
in VS_OUT                                                          
{                                                                  
    vec4 color;                                                    
} fs_in;                                                           
                                                                   
void main(void)                                                    
{                                                                  
    color = fs_in.color;
    //color = vec4(0.0, 1.0, 0.0, 1.0);
}  