'''
test for sorting tuples
'''

tuples = (
#   (id, edge label, last prefix, last among children, accepting state)
    (0,'','',True,False),
    (1,'a','', False, True),
    (3,'c','a', False, False),
    (8,'e','ac', False, False),
    (13,'s','ace', False, True),
    (9,'t','ac',True, True),
    (4,'m','a',True,False),
    (10,'y','am',True,True),
    (14,'a','amy',True,False),
    (17,'n','amya',True,True),
    (2,'e','',True,False),
    (5,'a','e',False,False),
    (11,'r','ea',True,True),
    )

print(tuples)
print(sorted(tuples))