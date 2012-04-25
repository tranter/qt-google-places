function getCurrentPointOfView() {
    return map.getBounds().getCenter().toUrlValue(6);
}
