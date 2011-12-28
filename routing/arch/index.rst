================================
Sistemos architektūros aprašymas
================================

Sistemos komponentai
====================

+   ``UnidirectionalCable (out: IPhysicalConnection)`` – virtualus,
    vienkryptis, dviejų klientų kabelis;
+   ``Link (in: IPhysicalConnection, out: ILink)`` – kanalinis lygis;
+   ``MAC (in: ILink, out: IMAC)`` – MAC polygis;
+   ``Network (in: IMAC, out: INetwork)`` – tinklo lygis;
+   ``Transport (in: INetwork, out: ITransport)`` – transporto lygis;
+   ``SMTPClient (in: ITransport)`` – taikomoji programa;
+   ``Router (in: IPhysicalConnection)`` – maršrutizatorius;
+   ``Computer (in: IPhysicalConnection)`` – kompiuteris.
