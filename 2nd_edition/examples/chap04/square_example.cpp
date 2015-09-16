class Square
{
public:
    int operator()(int x) const { return x * x; }
};

int main()
{
    Square square;
    int y = square(5);
    // y equals 25
}
