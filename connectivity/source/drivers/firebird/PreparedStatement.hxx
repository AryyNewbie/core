/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_FIREBIRD_PREPAREDSTATEMENT_HXX
#define INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_FIREBIRD_PREPAREDSTATEMENT_HXX

#include "Statement.hxx"

#include <cppuhelper/implbase5.hxx>

#include <com/sun/star/sdbc/XPreparedStatement.hpp>
#include <com/sun/star/sdbc/XParameters.hpp>
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#include <com/sun/star/sdbc/XPreparedBatchExecution.hpp>
#include <com/sun/star/io/XInputStream.hpp>

#include <ibase.h>

namespace connectivity
{
    namespace firebird
    {

        class OBoundParam;
        typedef ::cppu::ImplHelper5<    css::sdbc::XPreparedStatement,
                                        css::sdbc::XParameters,
                                        css::sdbc::XPreparedBatchExecution,
                                        css::sdbc::XResultSetMetaDataSupplier,
                                        css::lang::XServiceInfo> OPreparedStatement_Base;

        class OPreparedStatement :  public  OStatementCommonBase,
                                    public  OPreparedStatement_Base
        {
        protected:
            ::rtl::OUString                                       m_sSqlStatement;
            css::uno::Reference< css::sdbc::XResultSetMetaData >  m_xMetaData;

            XSQLDA*         m_pOutSqlda;
            XSQLDA*         m_pInSqlda;
            ::rtl::OUString                                       m_sTableName;
            void checkParameterIndex(sal_Int32 nParameterIndex)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException);

            /**
             * Set a numeric value in the input SQLDA. If the destination
             * parameter is not of nType then an Exception will be thrown.
             */
            template <typename T> void setValue(sal_Int32 nIndex, T& nValue, ISC_SHORT nType)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException);
            void setParameterNull(sal_Int32 nParameterIndex, bool bSetNull = true);

            void ensurePrepared()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException);
            /**
             * Assumes that all necessary mutexes have been taken.
             */
            void openBlobForWriting(isc_blob_handle& rBlobHandle, ISC_QUAD& rBlobId);
            /**
             * Assumes that all necessary mutexes have been taken.
             */
            void closeBlobAfterWriting(isc_blob_handle& rBlobHandle);

        protected:
            virtual void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle,
                                                                   const css::uno::Any& rValue)
                                                                        throw (css::uno::Exception, std::exception) override;
            virtual ~OPreparedStatement() override;
        public:
            DECLARE_SERVICE_INFO();
            // a constructor, which is required for returning objects:
            OPreparedStatement( Connection* _pConnection,
                                const ::rtl::OUString& sql);

            //XInterface
            virtual css::uno::Any SAL_CALL queryInterface( const css::uno::Type & rType ) throw(css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL acquire() throw() override;
            virtual void SAL_CALL release() throw() override;
            //XTypeProvider
            virtual css::uno::Sequence< css::uno::Type > SAL_CALL getTypes(  ) throw(css::uno::RuntimeException, std::exception) override;

            // XPreparedStatement
            virtual css::uno::Reference< css::sdbc::XResultSet > SAL_CALL
                executeQuery()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual sal_Int32 SAL_CALL
                executeUpdate()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual sal_Bool SAL_CALL
                execute()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual css::uno::Reference< css::sdbc::XConnection > SAL_CALL
                getConnection()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;

            // XParameters
            virtual void SAL_CALL setNull(sal_Int32 nIndex, sal_Int32 nValue)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setObjectNull(sal_Int32 parameterIndex, sal_Int32 sqlType, const ::rtl::OUString& typeName ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setBoolean( sal_Int32 nIndex, sal_Bool nValue)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setByte(sal_Int32 nIndex, sal_Int8 nValue)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setShort(sal_Int32 nIndex, sal_Int16 nValue)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setInt(sal_Int32 nIndex, sal_Int32 nValue)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setLong(sal_Int32 nIndex, sal_Int64 nValue)
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setFloat( sal_Int32 parameterIndex, float x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setDouble( sal_Int32 parameterIndex, double x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setString( sal_Int32 parameterIndex, const ::rtl::OUString& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setBytes( sal_Int32 parameterIndex, const css::uno::Sequence< sal_Int8 >& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setDate( sal_Int32 parameterIndex, const css::util::Date& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setTime( sal_Int32 parameterIndex, const css::util::Time& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setTimestamp( sal_Int32 parameterIndex, const css::util::DateTime& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setBinaryStream( sal_Int32 parameterIndex, const css::uno::Reference< css::io::XInputStream >& x, sal_Int32 length ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setCharacterStream( sal_Int32 parameterIndex, const css::uno::Reference< css::io::XInputStream >& x, sal_Int32 length ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setObject( sal_Int32 parameterIndex, const css::uno::Any& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setObjectWithInfo( sal_Int32 parameterIndex, const css::uno::Any& x, sal_Int32 targetSqlType, sal_Int32 scale ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setRef( sal_Int32 parameterIndex, const css::uno::Reference< css::sdbc::XRef >& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setBlob( sal_Int32 parameterIndex, const css::uno::Reference< css::sdbc::XBlob >& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setClob( sal_Int32 parameterIndex, const css::uno::Reference< css::sdbc::XClob >& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL setArray( sal_Int32 parameterIndex, const css::uno::Reference< css::sdbc::XArray >& x ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL clearParameters(  ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;

            // XPreparedBatchExecution -- UNSUPPORTED by firebird
            virtual void SAL_CALL
                addBatch()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual void SAL_CALL
                clearBatch()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            virtual css::uno::Sequence< sal_Int32 > SAL_CALL
                executeBatch()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;

            // XCloseable
            virtual void SAL_CALL close()
                throw(css::sdbc::SQLException,
                      css::uno::RuntimeException, std::exception) override;
            // OComponentHelper
            virtual void SAL_CALL disposing() override;

            // XResultSetMetaDataSupplier
            virtual css::uno::Reference< css::sdbc::XResultSetMetaData > SAL_CALL getMetaData(  ) throw(css::sdbc::SQLException, css::uno::RuntimeException, std::exception) override;

        };
    }
}
#endif // INCLUDED_CONNECTIVITY_SOURCE_DRIVERS_FIREBIRD_PREPAREDSTATEMENT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
