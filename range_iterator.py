import random


class RangeIterator:
    def __init__(self, a, b):
        self.a = a
        self.b = b
        self.used_numbers = set()

    def __iter__(self):
        return self

    def __next__(self):
        if self.a > self.b or len(self.used_numbers) == (self.b - self.a + 1):
            return None

        while True:
            num = random.randint(self.a, self.b)
            if num not in self.used_numbers:
                self.used_numbers.add(num)
                return num
