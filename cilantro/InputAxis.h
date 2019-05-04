#ifndef _INPUTAXIS_H_
#define _INPUTAXIS_H_

enum class InputAxisEnum {
    MouseX,
    MouseY
};

class InputAxis
{
    public:
        InputAxis();
        ~InputAxis();
        InputAxis(const InputAxis& other);
        InputAxis& operator=(const InputAxis& other);

        unsigned int GetCounter() { return m_Counter; }
        void SetCounter(unsigned int val) { m_Counter = val; }

    protected:

    private:
        unsigned int m_Counter;
};

#endif // _INPUTAXIS_H_
