
//===----------------------------------------------------------------------===//
//
//                         PelotonDB
//
// codegen_insert_test.cpp
//
// Identification: /peloton/test/codegen/codegen_insert_test.cpp
//
// Copyright (c) 2015, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>

#include "gtest/gtest.h"

#include "catalog/catalog.h"
#include "common/harness.h"
#include "common/logger.h"
#include "expression/tuple_value_expression.h"
#include "parser/insert_statement.h"
#include "parser/select_statement.h"
#include "planner/insert_plan.h"

#include "codegen/codegen_test_util.h"

namespace peloton {
namespace test {

//===--------------------------------------------------------------------===//
// Codegen Insert Tests
//===--------------------------------------------------------------------===//

class CodegenInsertTests : public PelotonCodeGenTest {};

TEST_F(CodegenInsertTests, InsertRecord) {
  catalog::Catalog::GetInstance();

  auto &txn_manager = concurrency::TransactionManagerFactory::GetInstance();
  auto txn = txn_manager.BeginTransaction();
  // Insert a table first
  auto id_column = catalog::Column(type::Type::INTEGER,
                                   type::Type::GetTypeSize(type::Type::INTEGER),
                                   "dept_id", true);
  auto name_column =
      catalog::Column(type::Type::VARCHAR, 32, "dept_name", false);

  std::unique_ptr<catalog::Schema> table_schema(
      new catalog::Schema({id_column, name_column}));

  catalog::Catalog::GetInstance()->CreateDatabase(DEFAULT_DB_NAME, txn);
  txn_manager.CommitTransaction(txn);

  txn = txn_manager.BeginTransaction();
  catalog::Catalog::GetInstance()->CreateTable(DEFAULT_DB_NAME, "TEST_TABLE",
                                               std::move(table_schema), txn);
  txn_manager.CommitTransaction(txn);

  auto table = catalog::Catalog::GetInstance()->GetTableWithName(
      DEFAULT_DB_NAME, "TEST_TABLE");

  txn = txn_manager.BeginTransaction();

//  std::unique_ptr<executor::ExecutorContext> context(
//      new executor::ExecutorContext(txn));

  std::unique_ptr<parser::InsertStatement> insert_node(
      new parser::InsertStatement(InsertType::VALUES));

  char *name = new char[11]();
  strcpy(name, "TEST_TABLE");
  auto table_ref = new parser::TableRef(TableReferenceType::NAME);
  parser::TableInfo *table_info = new parser::TableInfo();
  table_info->table_name = name;

  char *col_1 = new char[8]();
  strcpy(col_1, "dept_id");

  char *col_2 = new char[10]();
  strcpy(col_2, "dept_name");

  table_ref->table_info_ = table_info;
  insert_node->table_ref_ = table_ref;

  insert_node->columns = new std::vector<char *>;
  insert_node->columns->push_back(const_cast<char *>(col_1));
  insert_node->columns->push_back(const_cast<char *>(col_2));

  insert_node->insert_values =
      new std::vector<std::vector<expression::AbstractExpression *> *>;
  auto values_ptr = new std::vector<expression::AbstractExpression *>;
  insert_node->insert_values->push_back(values_ptr);

  values_ptr->push_back(new expression::ConstantValueExpression(
      type::ValueFactory::GetIntegerValue(70)));

  values_ptr->push_back(new expression::ConstantValueExpression(
      type::ValueFactory::GetVarcharValue("Hello")));

  insert_node->select = new parser::SelectStatement();

  planner::InsertPlan node(table, insert_node->columns,
                           insert_node->insert_values);
//  executor::InsertExecutor executor(&node, context.get());

  planner::BindingContext context;
  node.PerformBinding(context);
  codegen::BufferingConsumer buffer{{0, 1}, context};
  codegen::Query::RuntimeStats runtime_stats;
  CompileAndExecute(
      node, buffer, reinterpret_cast<char *>(buffer.GetState()),
      &runtime_stats);

  EXPECT_EQ(1, table->GetTupleCount());

  delete values_ptr->at(0);
  delete values_ptr->at(0);

  txn_manager.CommitTransaction(txn);

  // free the database just created
  txn = txn_manager.BeginTransaction();
  catalog::Catalog::GetInstance()->DropDatabaseWithName(DEFAULT_DB_NAME, txn);
  txn_manager.CommitTransaction(txn);
}

}  // End test namespace
}  // End peloton namespace
