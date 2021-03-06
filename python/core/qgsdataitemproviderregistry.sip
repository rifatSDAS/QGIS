/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgsdataitemproviderregistry.h                               *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsDataItemProviderRegistry
{
%Docstring
 This class keeps a list of data item providers that may add items to the browser tree.
 When created, it automatically adds providers from provider plugins (e.g. PostGIS, WMS, ...)

 QgsDataItemProviderRegistry is not usually directly created, but rather accessed through
 QgsApplication.dataItemProviderRegistry().

.. versionadded:: 2.10
%End

%TypeHeaderCode
#include "qgsdataitemproviderregistry.h"
%End
  public:

    QgsDataItemProviderRegistry();

    ~QgsDataItemProviderRegistry();


    QList<QgsDataItemProvider *> providers() const;
%Docstring
Get list of available providers
 :rtype: list of QgsDataItemProvider
%End

    void addProvider( QgsDataItemProvider *provider /Transfer/ );
%Docstring
Add a provider implementation. Takes ownership of the object.
%End

    void removeProvider( QgsDataItemProvider *provider );
%Docstring
Remove provider implementation from the list (provider object is deleted)
%End

  private:
    QgsDataItemProviderRegistry( const QgsDataItemProviderRegistry &rh );
};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgsdataitemproviderregistry.h                               *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
