#start

- character1:                       | `$charisma > 10.0` 
    Buenos días

- character2:  
    Buenos días `SC_Test` 

- character1:                       | `$t > 2.0 && ($life > 2.0 && ($charisma < 10.0 || $t > 1.0))`
    * quién es usted?               | `$t > 2.0 || ($life > 2.0 && (2.0 < $life || $life > 2.0) && 2.0 < $life)`
    * soy character1, usted es?     | `$t > 2.0 && ($life > 2.0 && ($charisma < 10.0 && $t > 1.0))`
    * [adios](#end) `1.wav`
    * [cómo van las cosas?](#cosas) `2.wav`     | `$r < 0.0`

- character2:
    soy character2

[end](#end) | `$l = 0.0`
```
$call NewMission 100 true
```
# cosas
- character2:
[mala](#end)

# end
