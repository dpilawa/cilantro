#ifndef _RENDERER_H_
#define _RENDERER_H_

class Renderer
{
public:
	Renderer ();
	~Renderer ();

	virtual void OnStart () = 0;
	virtual void OnFrame () = 0;
	virtual void OnEnd () = 0;

};

#endif
