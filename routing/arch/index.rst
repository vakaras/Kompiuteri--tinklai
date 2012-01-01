================================
Sistemos architektūros aprašymas
================================

MAC polygis
===========

Šio lygio užduotys:

+   pristatyti kadrą gavėjui.

Priemonės:

+   kadrų ribų atpažinimas naudojant bit-stuffing;
+   kadro korektiškumo patikrinimas naudojant CRC-16;
+   kadro priėmimas (atmetimas) pagal gavėjo adresą;
+   kadrų kolizijos apdorojimas panaudojant dvejetainį eksponentinio
    užlaikymo algoritmą.


LLC polygis
===========

Šio lygio užduotys:

+   persiųsti paketą gavėjui su patvirtinimu;
+   persiųsti nedidelį paketą (ne didesnį kaip 1 kadras) be patvirtinimo.

Priemonės:

+   paketų skaidymas į kadrus;
+   kadrų eiliškumo užtikrinimas panaudojant „sliding window“ algoritmą;
+   duomenų gautų iš skirtingų gavėjų atskyrimas (tai yra vienu metu
    turi būti galimybė priimti kadrus priklausančius skirtingų siuntėjų
    paketams).

Sistemos komponentai
====================

+   ``UnidirectionalCable (out: IPhysicalConnection)`` – virtualus,
    vienkryptis, dviejų klientų kabelis;
+   ``DataLink (in: IPhysicalConnection, out: ILink)`` – kanalinis lygis;
+   ``MAC (in: ILink, out: IMAC)`` – MAC polygis;
+   ``Network (in: IMAC, out: INetwork)`` – tinklo lygis;
+   ``Transport (in: INetwork, out: ITransport)`` – transporto lygis;
+   ``SMTPClient (in: ITransport)`` – taikomoji programa;
+   ``Router (in: IPhysicalConnection)`` – maršrutizatorius;
+   ``Computer (in: IPhysicalConnection)`` – kompiuteris.
