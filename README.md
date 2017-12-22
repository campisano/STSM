[![Build Status](https://travis-ci.org/campisano/STSM.svg?branch=master)](https://travis-ci.org/campisano/STSM)

# STSM (Spatio-Temporal Sequence Miner)

#### *Spatio-Temporal Sequence Miner* (*STSM*): a novel algorithm to discover tight sequential patterns in spatiotemporal sequences.

In a broad range of scientific and industrial applications, data is collected as spatiotemporal sequences, i.e, sequences of items with a time and a spatial position associated to each item. This leads to an appealing new challenge for this domain, namely i) frequent sequences constrained in space and time that may not be frequent in the entire dataset and ii) the time interval and space range where these sequences are frequent. We introduce a new Spatio-Temporal Sequence Miner (*STSM*) algorithm for such purpose. *STSM* first detects spatial ranges where sequences are frequent. Next, it composes all detected sequences inside each range to discover the ones constrained in space and time where these sequences are frequent.

## Example: Seismic use-case

Even though our solution is generic, we evaluate *STSM* on a seismic use case based on an open seismic survey ([Netherlands Offshore F3 Block](http://opendtect.org/osr/pmwiki.php/Main/NetherlandsOffshoreF3BlockComplete4GB "OpendTect website")) and the results ([Inline 401, SAX 10, spatial threshold 80%, time threshold 20%](http://aldebaran.eic.cefet-rj.br/seismic/results/inline-401_orientation-original/sax-10/img/spatial-80/block-20/stretch-0/ "Seismic use-case results")) indicate its ability to detect frequent sequences constrained in space and time. The identified sequences from *STSM* correspond to candidate areas for seismic *horizons* and *bright spots* that are of high value for domain experts. To the best of our knowledge, this is the first solution to tackle the problem of identifying frequent sequences constrained in space and time.

### Processing

![Alt text](/doc/README.md/seismic_use_case.jpg?raw=true "Seismic use-case processing")
Starting from a discretized spatio-temporal dataset, *STSM* first detects the spatial ranges that contains frequent sequential patterns, obtaining the solid ranged sequences. Then, it refines each detected spatial range looking for time intervals to detect the spatio-temporal blocks where the sequence is still frequent, obtaining the solid blocked sequences. The right side of this image shows potential bright spots identified using the proposed algorithm for inline 401, alphabet size 10, γ = 80%, and δ = 20%. The results follow the blue-yellow pattern produced using the previously known bright spots for this dataset.

### Ranked results

![Alt text](/doc/README.md/best_ranked_patterns.jpg?raw=true "Seismic best ranked results")
Identified sequences ranked according to their density for inline 401, alphabet size 10, solid range threshold γ 80% and solid block threshold δ 20%. Images from (a) to (d), (e) to (h), and (i) to (l) respectively present the four best ranked patterns for sequences of size two, three, and four.

### Notes

To reproduce the proposed experiment, download the [seismic dataset](https://opendtect.org/osr/pmwiki.php/Main/NetherlandsOffshoreF3BlockComplete4GB), get the [OpendTect software](https://dgbes.com/index.php/download) and export the 401 inline using the follow menu entry:
```
    Survey -> Export -> Seismic -> Simple File -> 3D
```

selecting the follow options:
```
    Input Cube:         1OriginalSeismic
    Volume Selection:   401/300-401/1250
    Put Sampling Info:  No
    Output File:        401.dat
```

Then, convert the 'dat' file to an usable 'CSV' file using the follow command:
```
    R --vanilla --slave --file=R/dat2csv_converter.r --args 401.dat 401.csv
```

Next, discretize the 'CSV' data using the follow command:
```
    R --vanilla --slave --file=R/sax_converter.r --args 401.csv 401_sax10.csv 10
```

And finally, run STSM with the follow command:
```
   ./BUILD/release/stsm -i 401_sax10.csv -o results.json -l stsm.log -s 80 -b 20
```

## License

![Alt text](/doc/README.md/COPYING.png?raw=true "License")

*STSM* is free software: you can redistribute it and/or modify it under the terms of the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl.html) as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
