1、Development 
   vs2015

2、lib Dependency
freetyped for font,link here:https://jaist.dl.sourceforge.net/project/freetype/freetype2/2.9.1/
glfw3:link:https://github.com/glfw/glfw

3、media:
ffmpeg for video: can download from link https://obsproject.com/downloads/dependencies2015.zip
OpenAL for audio: https://codeload.github.com/kcat/openal-soft/zip/master

4、3d model:
assimp for analysis 3d model:  https://github.com/assimp/assimp


5、Features
   all the error can be found in Console
   
   a、OGL Folder
   
      using c++ to make encapsulation of opengl.  
	  
	  more easy to deal with memory in opengl or Resources on disk 
	  
      shaders is using to process light in case we can have Unlimited number of light sources  in theory.
	  
   b、Graph Folder
   
      generate Geometric object like ball or cube with variable accuracy.
	  
   c、Imageproces
   
      encapsulation of Post-processing,convolution on shader ,in order to easy achieving image processing effect like gussian、sobel...
	  
   d、UI Folder
   
      create controls using opengl
	  
	  message dispatch and using tree struct to process controls message
	  
   e、Common Folder
   
      typelessness data like javascript‘s type "var"
	  
	  create signal solt as same as which working in QT.
	  
   f、Model Folder
   
      parsing and rendering 3d model,supported format like .3DS... as same as which is supported in assimp.
	  
	  tiny element is node，all node is corresponding to a setting data.
	  
   g、Media Folder
   
      parsing video and audio,support which is supported in ffmpeg,in loca like .avi or online like rtmp stream 
	  
	  we can easy making Media as a texture Benefited from using signal solt   
	  
   h、Test Folder 
   
      all the test is under this Folder,changing in GLUI.cpp
