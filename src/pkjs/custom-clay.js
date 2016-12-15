module.exports = function(minified) {
  var numberOfConnections = 10;
  var numberOfStations = 10;

  var clayConfig = this;

  function toggleAllProducts() {
    var allowAllProducts = this.get();
    var groupMembers = clayConfig.getItemsByGroup(this.config.group);
    var useDefaultProductsMessageKey = this.messageKey.replace('All', 'UseDef');
    for (var i = 0; i < groupMembers.length; i++) {
      var item = groupMembers[i];
      if (item.messageKey == this.messageKey || item.messageKey == useDefaultProductsMessageKey) {
         continue;
      }
      if (allowAllProducts) {
        item.hide();
      } else {
        item.show();
      }
    }
  }

  function toggleUseDefaultProducts() {
    var useDefaultProducts = this.get();
    if (useDefaultProducts) {
      hideOtherGroupMembersOf.call(this);
    } else {
      var allProdToggle = clayConfig.getItemByMessageKey(this.messageKey.replace('UseDef', 'All'));
      allProdToggle.show();
      toggleAllProducts.call(allProdToggle);
    }
  }
  
  function toggleGroupEnabled() {
    var enabled = this.get();
    var groupMembers = clayConfig.getItemsByGroup(this.config.group);
    for (var i = 0; i < groupMembers.length; i++) {
      var item = groupMembers[i];
      if (item.messageKey == this.messageKey) {
         continue;
      }
      if (enabled) {
        item.show();
      } else {
        item.hide();
      }
    }
    var useDefaultProductsToggle = clayConfig.getItemByMessageKey(this.messageKey.replace('Enabled', 'UseDefProd'));
    if (useDefaultProductsToggle) {
      if (enabled) {
        useDefaultProductsToggle.show();
      } else {
        useDefaultProductsToggle.hide();
      }
    }
  }
  
  function toggleHowto() {
    if (this.get()) {
      showOtherGroupMembersOf.call(this);
    } else {
      hideOtherGroupMembersOf.call(this);
    }
  }
  
  function hideOtherGroupMembersOf() {
    var groupMembers = clayConfig.getItemsByGroup(this.config.group);
    for (var i = 0; i < groupMembers.length; i++) {
      var item = groupMembers[i];
      if (item.id && this.id && item.id == this.id) {
         continue;
      } else if (item.messageKey && this.messageKey && item.messageKey == this.messageKey) {
         continue;
      }
      item.hide();
    }
  }
  
  function showOtherGroupMembersOf() {
    var groupMembers = clayConfig.getItemsByGroup(this.config.group);
    for (var i = 0; i < groupMembers.length; i++) {
      var item = groupMembers[i];
      if (item.id && this.id && item.id == this.id) {
         continue;
      } else if (item.messageKey && this.messageKey && item.messageKey == this.messageKey) {
         continue;
      }
      item.show();
    }
  }
  
  function swapItems(prefix, lhsIndex, rhsIndex) {
    var lhsSuffix = "[" + lhsIndex + "]";
    var rhsSuffix = "[" + rhsIndex + "]";
    var allItems = clayConfig.getAllItems();
    for (var i = 0; i < allItems.length; i++) {
      var rhsItem = allItems[i];
      if (!rhsItem.messageKey) {
        continue;
      }
      if (!rhsItem.messageKey.startsWith(prefix)) {
        continue;
      }
      if (!rhsItem.messageKey.endsWith(rhsSuffix)) {
        continue;
      }
      var lhsItem = clayConfig.getItemByMessageKey(rhsItem.messageKey.replace(rhsSuffix, lhsSuffix));
      if (!lhsItem) {
        continue;
      }
      var temp = lhsItem.get();
      lhsItem.set(rhsItem.get());
      rhsItem.set(temp);
    }
  }
  
  function truncateInput() {
    if (!this.config.attributes && !this.config.attributes.limit) {
      return;
    }
    var maxLength = this.config.attributes.limit;
    var value = this.get();
    if (value && value.length <= maxLength) {
      return;
    }
    this.set(value.substring(0, maxLength));
  }
  
  function handleUpButton() {
    var idParts = this.id.split("Up");
    var prefix = idParts[0];
    var sourceIndex = parseInt(idParts[1], 10);
    var destIndex = sourceIndex - 1;
    swapItems(prefix, destIndex, sourceIndex);
  }
  
  function installAllProductsToggle(prefix, suffix) {
    var allProdToggle = clayConfig.getItemByMessageKey(prefix + 'AllProd' + (suffix ? suffix : ''));
    toggleAllProducts.call(allProdToggle);
    allProdToggle.on('change', toggleAllProducts);
  }
  
  function installUseDefaultProductsToggle(prefix, suffix) {
    var useDefaultProdToggle = clayConfig.getItemByMessageKey(prefix + 'UseDefProd' + (suffix ? suffix : ''));
    toggleUseDefaultProducts.call(useDefaultProdToggle);
    useDefaultProdToggle.on('change', toggleUseDefaultProducts);
    useDefaultProdToggle.on('show', toggleUseDefaultProducts);
    useDefaultProdToggle.on('hide', hideOtherGroupMembersOf);
  }
  
  function installEnabledToggle(prefix, suffix) {
    var groupEnableToggle = clayConfig.getItemByMessageKey(prefix + 'Enabled' + (suffix ? suffix : ''));
    toggleGroupEnabled.call(groupEnableToggle);
    groupEnableToggle.on('change', toggleGroupEnabled);
  }
  
  function installUpButtonHandler(prefix, index) {
    var upButton = clayConfig.getItemById(prefix + 'Up' + index);
    if (upButton) {
      upButton.on('click', handleUpButton);
    }
  }
  
  function installShowHowtoHandler() {
    var howtoToggle = clayConfig.getItemById('HowtoToggle');
    toggleHowto.call(howtoToggle);
    howtoToggle.on('change', toggleHowto);
  }
  
  function installTruncationHandlers() {
    var allInputItems = clayConfig.getItemsByType('input');
    for (var i = 0; i < allInputItems.length; i++) {
      var inputItem = allInputItems[i];
      inputItem.on('change', truncateInput);
    }
  }
  
  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    installAllProductsToggle('Def');
    var indexSuffix, i;
    for (i = 0; i < numberOfStations; i++) {
      indexSuffix = "[" + i + "]";
      installAllProductsToggle('Stat', indexSuffix);
      installUseDefaultProductsToggle('Stat', indexSuffix);
      installEnabledToggle('Stat', indexSuffix);
      installUpButtonHandler('Stat', i);
    }
    for (i = 0; i < numberOfConnections; i++) {
      indexSuffix = "[" + i + "]";
      installAllProductsToggle('Conn', indexSuffix);
      installUseDefaultProductsToggle('Conn', indexSuffix);
      installEnabledToggle('Conn', indexSuffix);
      installUpButtonHandler('Conn', i);
    }
    installShowHowtoHandler();
    installTruncationHandlers();
  });
};