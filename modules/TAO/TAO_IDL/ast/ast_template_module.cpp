// $Id$

#include "ast_template_module.h"
#include "ast_constant.h"
#include "ast_visitor.h"

#include "utl_err.h"
#include "global_extern.h"

AST_Template_Module::AST_Template_Module (
      UTL_ScopedName *n,
      FE_Utils::T_PARAMLIST_INFO *template_params)
  : COMMON_Base (false,
                 false),
    AST_Decl (AST_Decl::NT_module,
              n),
    UTL_Scope (AST_Decl::NT_module),
    AST_Module (n),
    AST_Type (AST_Decl::NT_module,
              n),
    template_params_ (template_params)
{
}

AST_Template_Module::~AST_Template_Module (void)
{
}

FE_Utils::T_PARAMLIST_INFO const *
AST_Template_Module::template_params (void) const
{
  return this->template_params_;
}

bool
AST_Template_Module::match_arg_names (FE_Utils::T_ARGLIST *args)
{
  if (args->size () != this->template_params_->size ())
    {
      idl_global->err ()->error1 (UTL_Error::EIDL_T_ARG_LENGTH,
                                  this);
      return false;
    }
    
  size_t slot = 0UL;
  
  for (FE_Utils::T_ARGLIST::CONST_ITERATOR i (*args);
       !i.done ();
       i.advance (), ++slot)
    {
      AST_Decl **item = 0;
      i.next (item);
      AST_Decl *d = *item;
      
      if (d->node_type () == AST_Decl::NT_typedef)
        {
          AST_Typedef *td =
            AST_Typedef::narrow_from_decl (d);
            
          d = td->primitive_base_type ();
        }
        
      FE_Utils::T_Param_Info *param = 0;
      (void) this->template_params_->get (param, slot);
      const char *s = 0;
      
      if (! this->match_param_type (param, d))
        {
          UTL_ScopedName *n = d->name ();
          
          if (n == 0)
            {
              AST_Constant *c =
                AST_Constant::narrow_from_decl (d);
                
              s = c->exprtype_to_string ();
            }
          else
            {
              s = d->full_name ();
            }
        
          idl_global->err ()->mismatched_template_param (s);
            
          return false;
        }
    }
    
  return true;
}

void
AST_Template_Module::destroy (void)
{
  delete this->template_params_;
  this->template_params_ = 0;

  this->AST_Module::destroy ();
}

int
AST_Template_Module::ast_accept (ast_visitor *)
{
  return 0;//visitor->visit_template_module (this);
}

void
AST_Template_Module::dump (ACE_OSTREAM_TYPE & /* o */)
{
}

bool
AST_Template_Module::match_param_type (FE_Utils::T_Param_Info *param,
                                       AST_Decl *d)
{
  if (param->type_ == AST_Decl::NT_type)
    {
      return true;
    }
    
  if (d->node_type () == AST_Decl::NT_typedef)
    {
      AST_Typedef *td = AST_Typedef::narrow_from_decl (d);
      d = td->primitive_base_type ();
    }
    
  AST_Decl::NodeType other_type = d->node_type ();
    
  if (other_type == AST_Decl::NT_const)
    {
      AST_Constant *c =
        AST_Constant::narrow_from_decl (d);
        
      AST_Expression *ex = c->constant_value ();
       
      AST_Expression::AST_ExprValue *ev =
        ex->check_and_coerce (param->const_type_,
                              param->enum_const_type_decl_);
        
      if (ev == 0)
        {
          idl_global->err ()->coercion_error (ex,
                                              param->const_type_);
        }
        
      return (ev != 0);
    }
    
  return (param->type_ == other_type);
}

IMPL_NARROW_FROM_DECL (AST_Template_Module)
IMPL_NARROW_FROM_SCOPE (AST_Template_Module)

