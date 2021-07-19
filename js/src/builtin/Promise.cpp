  return true;
}

static MOZ_MUST_USE bool PerformPromiseAllSettled(JSContext *cx, JS::ForOfIterator& iterator,
                                                  HandleObject C, HandleObject promiseObj,
                                                  HandleObject resolve, HandleObject reject,
                                                  bool* done);

// ES2020
static bool
Promise_static_allSettled(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);
    RootedValue iterable(cx, args.get(0));
    RootedValue CVal(cx, args.thisv());
    if (!CVal.isObject()) {
        JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr, JSMSG_NOT_NONNULL_OBJECT,
                                  "Receiver of Promise.allSettled call");
        return false;
    }

    RootedObject C(cx, &CVal.toObject());
    RootedObject resultPromise(cx);
    RootedObject resolve(cx);
    RootedObject reject(cx);
    if (!NewPromiseCapability(cx, C, &resultPromise, &resolve, &reject, false))
        return false;

    JS::ForOfIterator iter(cx);
    if (!iter.init(iterable, JS::ForOfIterator::AllowNonIterable))
        return AbruptRejectPromise(cx, args, resultPromise, reject);

    if (!iter.valueIsIterable()) {
        JS_ReportErrorNumberASCII(cx, GetErrorMessage, nullptr, JSMSG_NOT_ITERABLE,
                                  "Argument of Promise.allSettled");
        return AbruptRejectPromise(cx, args, resultPromise, reject);
    }

    bool done;
    bool result = PerformPromiseAllSettled(cx, iter, C, resultPromise, resolve, reject, &done);

    if (!result) {
        if (!done)
            iter.closeThrow();

        return AbruptRejectPromise(cx, args, resultPromise, reject);
    }

    args.rval().setObject(*resultPromise);
    return true;
}

static MOZ_MUST_USE bool PerformPromiseThen(JSContext* cx, Handle<PromiseObject*> promise,
                                            HandleValue onFulfilled_, HandleValue onRejected_,
                                            HandleObject resultPromise,
                                            HandleObject resolve, HandleObject reject);

static bool PromiseAllResolveElementFunction(JSContext* cx, unsigned argc, Value* vp);
static bool PromiseAllSettledResolveElementFunction(JSContext* cx, unsigned argc, Value* vp);
static bool PromiseAllSettledRejectElementFunction(JSContext* cx, unsigned argc, Value* vp);

// Unforgeable version of ES2016, 25.4.4.1.
MOZ_MUST_USE JSObject*
@@ -1848,6 +1899,120 @@ PerformPromiseAll(JSContext *cx, JS::ForOfIterator& iterator, HandleObject C,
    }
}

static MOZ_MUST_USE bool
PerformPromiseAllSettled(JSContext *cx, JS::ForOfIterator& iterator, HandleObject C,
                         HandleObject promiseObj, HandleObject resolve, HandleObject reject,
                         bool* done)
{
    *done = false;

    RootedObject unwrappedPromiseObj(cx);
    if (IsWrapper(promiseObj)) {
        unwrappedPromiseObj = CheckedUnwrap(promiseObj);
        MOZ_ASSERT(unwrappedPromiseObj);
    }

    RootedValue CVal(cx, ObjectValue(*C));

    RootedObject valuesArray(cx);
    if (unwrappedPromiseObj) {
        JSAutoCompartment ac(cx, unwrappedPromiseObj);
        valuesArray = NewDenseFullyAllocatedArray(cx, 0);
    } else {
        valuesArray = NewDenseFullyAllocatedArray(cx, 0);
    }
    if (!valuesArray)
        return false;

    RootedValue valuesArrayVal(cx, ObjectValue(*valuesArray));
    if (!cx->compartment()->wrap(cx, &valuesArrayVal))
        return false;

    Rooted<PromiseAllDataHolder*> dataHolder(cx, NewPromiseAllDataHolder(cx, promiseObj,
                                                                         valuesArrayVal, resolve));
    if (!dataHolder)
        return false;
    RootedValue dataHolderVal(cx, ObjectValue(*dataHolder));

    uint32_t index = 0;

    RootedValue nextValue(cx);
    RootedId indexId(cx);

    while (true) {
        if (!iterator.next(&nextValue, done)) {
            *done = true;
            return false;
        }

        if (*done) {
            int32_t remainingCount = dataHolder->decreaseRemainingCount();
            if (remainingCount == 0) {
                return RunResolutionFunction(cx, resolve, valuesArrayVal, ResolveMode,
                                             promiseObj);
            }
            return true;
        }

        { // Scoped for AutoCompartment
            JSAutoCompartment ac(cx, valuesArray);
            indexId = INT_TO_JSID(index);
            if (!DefineProperty(cx, valuesArray, indexId, UndefinedHandleValue))
                return false;
        }

        RootedValue nextPromise(cx);
        RootedValue staticResolve(cx);
	RootedValue staticReject(cx);

	// Because Promise.allSettled can continue whether the promise is fulfilled or rejected, we 
	// should only return false if neither condition is true.

        if (!GetProperty(cx, CVal, cx->names().resolve, &staticResolve) && 
	    !GetProperty(cx, CVal, cx->names().reject, &staticReject))
            return false;

        FixedInvokeArgs<1> resolveArgs(cx);
        resolveArgs[0].set(nextValue);
        FixedInvokeArgs<1> rejectArgs(cx);
        rejectArgs[0].set(nextValue);
        if (!Call(cx, staticResolve, CVal, resolveArgs, &nextPromise) && 
	    !Call(cx, staticReject, CVal, rejectArgs, &nextPromise))
            return false;

		
	RootedFunction resolveFunc(cx, NewNativeFunction(cx, PromiseAllSettledResolveElementFunction,
			           1, nullptr,
			           gc::AllocKind::FUNCTION_EXTENDED,
			           GenericObject));
		
	RootedFunction rejectFunc(cx, NewNativeFunction(cx, PromiseAllSettledRejectElementFunction,
				  1, nullptr,
				  gc::AllocKind::FUNCTION_EXTENDED,
				  GenericObject));
	if (!resolveFunc && !rejectFunc) {
	     return false;
	}
		
	resolveFunc->setExtendedSlot(PromiseAllResolveElementFunctionSlot_Data, dataHolderVal);
	resolveFunc->setExtendedSlot(PromiseAllResolveElementFunctionSlot_ElementIndex,
				Int32Value(index));
	rejectFunc->setExtendedSlot(PromiseAllResolveElementFunctionSlot_Data, dataHolderVal);
	rejectFunc->setExtendedSlot(PromiseAllResolveElementFunctionSlot_ElementIndex,
				Int32Value(index));

        dataHolder->increaseRemainingCount();

        RootedValue resolveFunVal(cx, ObjectValue(*resolveFunc));
        RootedValue rejectFunVal(cx, ObjectValue(*rejectFunc));
        if (!BlockOnPromise(cx, nextPromise, promiseObj, resolveFunVal, rejectFunVal))
            return false;

        index++;
        MOZ_ASSERT(index > 0);
    }
}

// ES2016, 25.4.4.1.2.
static bool
PromiseAllResolveElementFunction(JSContext* cx, unsigned argc, Value* vp)
@@ -1921,6 +2086,159 @@ PromiseAllResolveElementFunction(JSContext* cx, unsigned argc, Value* vp)
    return true;
}

// ES2020.
static bool
PromiseAllSettledResolveElementFunction(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    RootedFunction resolve(cx, &args.callee().as<JSFunction>());
    RootedValue xVal(cx, args.get(0));
    RootedValue dataVal(cx, resolve->getExtendedSlot(PromiseAllResolveElementFunctionSlot_Data));

    if (dataVal.isUndefined()) {
        args.rval().setUndefined();
        return true;
    }

    Rooted<PromiseAllDataHolder*> data(cx, &dataVal.toObject().as<PromiseAllDataHolder>());

    resolve->setExtendedSlot(PromiseAllResolveElementFunctionSlot_Data, UndefinedValue());

    int32_t index = resolve->getExtendedSlot(PromiseAllResolveElementFunctionSlot_ElementIndex)
                    .toInt32();

    RootedValue valuesVal(cx, data->valuesArray());
    RootedObject valuesObj(cx, &valuesVal.toObject());
    bool valuesListIsWrapped = false;
    if (IsWrapper(valuesObj)) {
        valuesListIsWrapped = true;
        // See comment for PerformPromiseAll, step 3 for why we unwrap here.
        valuesObj = UncheckedUnwrap(valuesObj);
    }
    NativeObject* values = &valuesObj->as<NativeObject>();

    // The index is guaranteed to be initialized to `undefined`.
    if (valuesListIsWrapped) {
        AutoCompartment ac(cx, values);
        if (!cx->compartment()->wrap(cx, &xVal))
            return false;
    }
    
    RootedPlainObject obj(cx, NewBuiltinClassInstance<PlainObject>(cx));
    if (!obj) {
        return false;
    }
    RootedId id(cx, NameToId(cx->names().status));
    RootedValue statusValue(cx);
    statusValue.setString(cx->names().fulfilled);
    if (!::JS_DefinePropertyById(cx, obj, id, statusValue, JSPROP_ENUMERATE)) {
      return false;
    }
    id = NameToId(cx->names().value);
    if (!::JS_DefinePropertyById(cx, obj, id, xVal, JSPROP_ENUMERATE)) {
      return false;
    }

    RootedValue objVal(cx, ObjectValue(*obj));
/*  if (needsWrapping) {
    AutoRealm ar(cx, valuesObj);
    if (!cx->compartment()->wrap(cx, &objVal)) {
      return false;
    }
  }    */
    values->setDenseElement(index, objVal);

    uint32_t remainingCount = data->decreaseRemainingCount();

    if (remainingCount == 0) {
        RootedObject resolveAllFun(cx, data->resolveObj());
        RootedObject promiseObj(cx, data->promiseObj());
        if (!RunResolutionFunction(cx, resolveAllFun, valuesVal, ResolveMode, promiseObj))
            return false;
    }

    args.rval().setUndefined();
    return true;
}

static bool
PromiseAllSettledRejectElementFunction(JSContext* cx, unsigned argc, Value* vp)
{
    CallArgs args = CallArgsFromVp(argc, vp);

    RootedFunction resolve(cx, &args.callee().as<JSFunction>());
    RootedValue xVal(cx, args.get(0));
    RootedValue dataVal(cx, resolve->getExtendedSlot(PromiseAllResolveElementFunctionSlot_Data));

    if (dataVal.isUndefined()) {
        args.rval().setUndefined();
        return true;
    }

    Rooted<PromiseAllDataHolder*> data(cx, &dataVal.toObject().as<PromiseAllDataHolder>());

    resolve->setExtendedSlot(PromiseAllResolveElementFunctionSlot_Data, UndefinedValue());

    int32_t index = resolve->getExtendedSlot(PromiseAllResolveElementFunctionSlot_ElementIndex)
                    .toInt32();

    RootedValue valuesVal(cx, data->valuesArray());
    RootedObject valuesObj(cx, &valuesVal.toObject());
    bool valuesListIsWrapped = false;
    if (IsWrapper(valuesObj)) {
        valuesListIsWrapped = true;
        // See comment for PerformPromiseAll, step 3 for why we unwrap here.
        valuesObj = UncheckedUnwrap(valuesObj);
    }
    NativeObject* values = &valuesObj->as<NativeObject>();

    // The index is guaranteed to be initialized to `undefined`.
    if (valuesListIsWrapped) {
        AutoCompartment ac(cx, values);
        if (!cx->compartment()->wrap(cx, &xVal))
            return false;
    }
    
    RootedPlainObject obj(cx, NewBuiltinClassInstance<PlainObject>(cx));
    if (!obj) {
        return false;
    }
    RootedId id(cx, NameToId(cx->names().status));
    RootedValue statusValue(cx);
    statusValue.setString(cx->names().rejected);
    if (!::JS_DefinePropertyById(cx, obj, id, statusValue, JSPROP_ENUMERATE)) {
      return false;
    }
    RootedValue resultValue(cx);
    id = NameToId(cx->names().reason);
    if (!::JS_DefinePropertyById(cx, obj, id, xVal, JSPROP_ENUMERATE)) {
      return false;
    }

    RootedValue objVal(cx, ObjectValue(*obj));
/*  if (needsWrapping) {
    AutoRealm ar(cx, valuesObj);
    if (!cx->compartment()->wrap(cx, &objVal)) {
      return false;
    }
  }    */
    values->setDenseElement(index, objVal);    


    uint32_t remainingCount = data->decreaseRemainingCount();

    if (remainingCount == 0) {
        RootedObject resolveAllFun(cx, data->resolveObj());
        RootedObject promiseObj(cx, data->promiseObj());
        if (!RunResolutionFunction(cx, resolveAllFun, valuesVal, ResolveMode, promiseObj))
            return false;
    }

    args.rval().setUndefined();
    return true;
}

static MOZ_MUST_USE bool PerformPromiseRace(JSContext *cx, JS::ForOfIterator& iterator,
                                            HandleObject C, HandleObject promiseObj,
                                            HandleObject resolve, HandleObject reject,
@@ -3246,6 +3564,7 @@ static const JSPropertySpec promise_properties[] = {

static const JSFunctionSpec promise_static_methods[] = {
    JS_FN("all", Promise_static_all, 1, 0),
    JS_FN("allSettled", Promise_static_allSettled, 1, 0),
    JS_FN("race", Promise_static_race, 1, 0),
    JS_FN("reject", Promise_reject, 1, 0),
    JS_FN("resolve", Promise_static_resolve, 1, 0),


+ 1
- 0

View File
@@ -325,6 +325,7 @@
    macro(startTimestamp, startTimestamp, "startTimestamp") \
    macro(state, state, "state") \
    macro(static, static_, "static") \
    macro(status, status, "status") \
    macro(std_Function_apply, std_Function_apply, "std_Function_apply") \
    macro(sticky, sticky, "sticky") \
    macro(StringIterator, StringIterator, "String Iterator") \


