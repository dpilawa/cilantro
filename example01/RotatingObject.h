class RotatingObject : public GameObject
{
public:
	RotatingObject ();
	~RotatingObject ();

	void OnFrame ();
private:
	// rotation speed in degrees per second
	float rotationSpeed;
};

