/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/gps/qgsgpsdconnection.h                                     *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsGpsdConnection: QgsNMEAConnection
{
%Docstring
 Evaluates NMEA sentences coming from gpsd*
%End

%TypeHeaderCode
#include "qgsgpsdconnection.h"
%End
  public:
    QgsGpsdConnection( const QString &host, qint16 port, const QString &device );
    ~QgsGpsdConnection();

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/gps/qgsgpsdconnection.h                                     *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
