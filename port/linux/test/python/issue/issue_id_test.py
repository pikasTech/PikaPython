class Dog:
    def __init__(self, name) -> None:
        self.name = name

    def eat(self):
        print('self id = ', id(self))
        print('%s eat...' % (self.name))

dog = Dog('dog1')
print('dog1 id = ', id(dog))
dog.eat()


def test():
    dog = Dog('dog2')
    print('dog2 id = ', id(dog))
    dog.eat()


test()
