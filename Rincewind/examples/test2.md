#start

-character1: | $charisma > 10
Buenos días

-character2:
Buenos días

-character1:    | $life > 2
*quién es usted? | $t > 2
*soy character1, usted es?
*[adios](#end)
*[cómo van las cosas?](#cosas) | $r < 0

-character2:
soy character2

[end](#end) | $l = 0
```
$call NewMission 100 true
```
#cosas
-character2:
[mal](#end)

#end