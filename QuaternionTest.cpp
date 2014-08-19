#include <iostream>
#include <array>


class Quaternion
{
private:
	std::array<float, 4> _data;
public:
	Quaternion(float q0, float q1, float q2, float q3)
	{
		_data[0] = q0;
		_data[1] = q1;
		_data[2] = q2;
		_data[3] = q3;
	}
	Quaternion(std::array<float, 4> data) : _data(data)
	{}
	Quaternion(const Quaternion &q) : _data(q._data)
	{}
	float& operator[](std::size_t i)
	{
		return _data[i];
	}
	float operator[](std::size_t i) const
	{
		return _data[i];
	}
	void apply(const Quaternion &q)
	{
		std::array<float, 4> Q(_data);
		_data[0] = (Q[0] * q[0]) - (Q[1] * q[1]) - (Q[2] * q[2]) - (Q[3] * q[3]);
		_data[1] = (Q[0] * q[1]) + (Q[1] * q[0]) + (Q[2] * q[3]) - (Q[3] * q[2]);
		_data[2] = (Q[0] * q[2]) - (Q[1] * q[3]) + (Q[2] * q[0]) + (Q[3] * q[1]);
		_data[3] = (Q[0] * q[3]) + (Q[1] * q[2]) - (Q[2] * q[1]) + (Q[3] * q[0]);
	}
	void print() const
	{
		std::cout << "(" << _data[0] << "," << _data[1] << "," << _data[2] << "," << _data[3] << ")" << std::endl;
	}
};



int main(int argc, char** argv)
{
	
	Quaternion q1(2, 3, 4, 5);
	Quaternion q2(3, 4, 5, 6);
	q1.print();
	q2.print();
	q1.apply(q2);
	q1.print();

	while (1);
	return 0;
}

