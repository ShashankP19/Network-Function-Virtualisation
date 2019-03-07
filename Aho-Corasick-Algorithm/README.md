### Usage
`gcc test/main.c src/acs.c src/queue.c -Wall -o acs.out`

`./acs.out <List of space separated keywords>`

### Example
```
./acs.out test key random words hel
Enter input string: ran
======No Match Found=====
stskelhel
Found Keyword: hel from 6 to 8
randomwords        
Found Keyword: random from 0 to 5
Found Keyword: words from 6 to 10
```

### References
- [String Matching: An Aid to Bibliographic Search ](https://cr.yp.to/bib/1975/aho.pdf)