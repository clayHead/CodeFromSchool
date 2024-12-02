# 4061-proj2
The second project in CSCI 4061 at University of Minnesota on File Systems

The description for the project can be found in the pdf in the respository.

|Members| x500 |
|-------|------|
|Chase | joh13266|
| Clayton| joh13124|
| Modou | jawxx001|


## Contributions:

|Member|Contribution|
|-------|-----------|
|Clayton|du and redirection assistance|
|Modou  |Grep and cd |
|Chase | Shell parsing, execution, piping, redirection<sup>*</sup>|

<sup>*</sup>Only in cases of single pipe or redirection throughout a single input.
## Known Bugs:

Pipe and redirects do not properly output to a file when used in combination.

```bash
cat Makefile | grep util >> out.txt
cat out.txt
```
out.txt is now an empty file.

## Test Cases Used:

```bash
du ./
cat Makefile | grep util
cat Makefile | grep util >> out.txt
cat Makefile >> out.txt
cat Makefile > out.txt
cd ..
```
## Extra Credit:

No extra credit was attempted