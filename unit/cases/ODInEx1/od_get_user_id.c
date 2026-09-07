#define UT_CUSTOM_MOCK_ODSyncPublicCallAllowed

static BOOL ut_calls_allowed;

BOOL utm_ODSyncPublicCallAllowed(void)
{
   return ut_calls_allowed;
}

static void reset_user_id(void)
{
   memset(&od_control, 0, sizeof(od_control));
   ut_calls_allowed = TRUE;
   pszBBSDevUserID = NULL;
}

static void returns_bbsdev_key(void)
{
   reset_user_id();
   od_control.od_info_type = BBSDEVDRP;
   pszBBSDevUserID = (char *)"opaque:key/42";
   UT_ASSERT(strcmp("opaque:key/42", utt_od_get_user_id()) == 0);
   pszBBSDevUserID = NULL;
   UT_ASSERT(strcmp("", utt_od_get_user_id()) == 0);
}

static void combines_legacy_fields(void)
{
   static const BYTE numbered[] = {
      EXITINFO, RA1EXITINFO, CHAINTXT, SFDOORSDAT, DOORSYS_GAP,
      QBBS275EXITINFO, DOORSYS_WILDCAT, RA2EXITINFO, TRIBBSSYS, DOOR32SYS
   };
   unsigned index;
   reset_user_id();
   od_control.user_num = 0;
   strcpy(od_control.user_name, "Jane Doe");
   for(index = 0; index < DIM(numbered); ++index)
   {
      od_control.od_info_type = numbered[index];
      UT_ASSERT(strcmp("0:Jane Doe", utt_od_get_user_id()) == 0);
   }

   od_control.user_num = 42;
   UT_ASSERT(strcmp("42:Jane Doe", utt_od_get_user_id()) == 0);

   od_control.user_name[0] = '\0';
   strcpy(od_control.user_handle, "Jane");
   UT_ASSERT(strcmp("42:Jane", utt_od_get_user_id()) == 0);

   od_control.user_handle[0] = '\0';
   UT_ASSERT(strcmp("42:", utt_od_get_user_id()) == 0);
}

static void omits_unavailable_number(void)
{
   static const BYTE unnumbered[] = {
      DORINFO1, CALLINFO, DOORSYS_DRWY, NO_DOOR_FILE
   };
   unsigned index;
   reset_user_id();
   od_control.user_num = 42;
   strcpy(od_control.user_name, "Jane Doe");
   for(index = 0; index < DIM(unnumbered); ++index)
   {
      od_control.od_info_type = unnumbered[index];
      UT_ASSERT(strcmp(":Jane Doe", utt_od_get_user_id()) == 0);
   }

   od_control.user_name[0] = '\0';
   strcpy(od_control.user_handle, "Jane");
   UT_ASSERT(strcmp(":Jane", utt_od_get_user_id()) == 0);

   od_control.user_handle[0] = '\0';
   UT_ASSERT(strcmp("", utt_od_get_user_id()) == 0);
}

static void handles_custom_and_denied_calls(void)
{
   reset_user_id();
   od_control.od_info_type = CUSTOM;
   od_control.user_num = 7;
   strcpy(od_control.user_name, "Jane");
   UT_ASSERT(strcmp("7:Jane", utt_od_get_user_id()) == 0);

   od_control.user_num = 0;
   UT_ASSERT(strcmp(":Jane", utt_od_get_user_id()) == 0);

   ut_calls_allowed = FALSE;
   UT_ASSERT(strcmp("", utt_od_get_user_id()) == 0);
}

static const UTTestCase ut_cases[] = {
   {"BBSDEV", returns_bbsdev_key},
   {"legacy", combines_legacy_fields},
   {"no number", omits_unavailable_number},
   {"custom and denied", handles_custom_and_denied_calls}
};
