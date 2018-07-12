
class Orbiter : public GameObject
{
public:
	Orbiter ();
	Orbiter (float period);
	~Orbiter ();

	void OnFrame ();
private:

	// orbiting period in days
	float orbitingPeriod;
};

