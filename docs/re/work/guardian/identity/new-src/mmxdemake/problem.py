from dataclasses import dataclass


@dataclass(frozen=True)
class Problem:
    where: str
    message: str

    def __str__(self):
        return f"{self.where}: {self.message}"
